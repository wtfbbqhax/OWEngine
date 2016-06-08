////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  This file contains some code from kwaak3:
//  Copyright (C) 2010 Roderick Colenbrander
//  Copyright (C) 2012 n0n3m4
//  Copyright (C) 2016 Dusan Jocic <dusanjocic@msn.com>
//
//  OWEngine source code is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  OWEngine source code is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA,
//  or simply visit <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------------
//  File name:   owe.c
//  Version:     v1.00
//  Created:
//  Compilers:   
//  Description:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include <owe.h>

int  (*qmain)(int argc, char **argv);
void (*onFrame)();
void (*onKeyEvent)(int state, int key,int chr);
void (*onMotionEvent)(float x, float y);
void (*onAudio)();
void (*setCallbacks)(void *func, void *func2, void *func3);
void (*setResolution)(int width, int height);
void (*vidRestart)();

jmethodID android_initAudio;
jmethodID android_writeAudio;
jmethodID android_setState;

static JavaVM *jVM;
static jobject audioBuffer=0;
static jobject oweCallbackObj=0;
static void *libdl;

static void loadLib(char* libpath)
{
    libdl = dlopen(libpath, RTLD_NOW | RTLD_GLOBAL);
    if(!libdl)
    {
        __android_log_print(ANDROID_LOG_ERROR, "OWE_JNI", "Unable to load library: %s\n", dlerror());
        return;
    }
    qmain = dlsym(libdl, "main");
    onFrame = dlsym(libdl, "OWE_DrawFrame");
    onKeyEvent = dlsym(libdl, "OWE_KeyEvent");
    onMotionEvent = dlsym(libdl, "OWE_MotionEvent");
    onAudio = dlsym(libdl, "OWE_GetAudio");
    setCallbacks = dlsym(libdl, "OWE_SetCallbacks");
    setResolution = dlsym(libdl, "OWE_SetResolution");
	vidRestart = dlsym(libdl, "OWE_OGLRestart");
}

void initAudio(void *buffer, int size)
{
    JNIEnv *env;
    jobject tmp;
    (*jVM)->GetEnv(jVM, (void**) &env, JNI_VERSION_1_4);
    tmp = (*env)->NewDirectByteBuffer(env, buffer, size);
    audioBuffer = (jobject)(*env)->NewGlobalRef(env, tmp);
    return (*env)->CallVoidMethod(env, oweCallbackObj, android_initAudio, size);
}

void writeAudio(int offset, int length)
{
	if (audioBuffer==0) return;
    JNIEnv *env;
    if (((*jVM)->GetEnv(jVM, (void**) &env, JNI_VERSION_1_4))<0)
    {
    	(*jVM)->AttachCurrentThread(jVM,&env, NULL);
    }
    (*env)->CallVoidMethod(env, oweCallbackObj, android_writeAudio, audioBuffer, offset, length);
}

void setState(int state)
{
    JNIEnv *env;
    (*jVM)->GetEnv(jVM, (void**) &env, JNI_VERSION_1_4);
    (*env)->CallVoidMethod(env, oweCallbackObj, android_setState, state);
}

int JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv *env;
    jVM = vm;
    if((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        __android_log_print(ANDROID_LOG_ERROR, "OWE_JNI", "JNI fatal error");
        return -1;
    }

    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL Java_com_owengine_owe_OWEJNI_setCallbackObject(JNIEnv *env, jclass c, jobject obj)
{
    oweCallbackObj = obj;
    jclass oweCallbackClass;

    (*jVM)->GetEnv(jVM, (void**) &env, JNI_VERSION_1_4);
    oweCallbackObj = (jobject)(*env)->NewGlobalRef(env, obj);
    oweCallbackClass = (*env)->GetObjectClass(env, oweCallbackObj);
    
    android_initAudio = (*env)->GetMethodID(env,oweCallbackClass,"init","(I)V");
    android_writeAudio = (*env)->GetMethodID(env,oweCallbackClass,"writeAudio","(Ljava/nio/ByteBuffer;II)V");
	android_setState = (*env)->GetMethodID(env,oweCallbackClass,"setState","(I)V");
}

static void UnEscapeQuotes( char *arg )
{
	char *last = NULL;
	while( *arg ) {
		if( *arg == '"' && *last == '\\' ) {
			char *c_curr = arg;
			char *c_last = last;
			while( *c_curr ) {
				*c_last = *c_curr;
				c_last = c_curr;
				c_curr++;
			}
			*c_last = '\0';
		}
		last = arg;
		arg++;
	}
}

static int ParseCommandLine(char *cmdline, char **argv)
{
	char *bufp;
	char *lastp = NULL;
	int argc, last_argc;
	argc = last_argc = 0;
	for ( bufp = cmdline; *bufp; ) {		
		while ( isspace(*bufp) ) {
			++bufp;
		}		
		if ( *bufp == '"' ) {
			++bufp;
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}			
			while ( *bufp && ( *bufp != '"' || *lastp == '\\' ) ) {
				lastp = bufp;
				++bufp;
			}
		} else {
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}			
			while ( *bufp && ! isspace(*bufp) ) {
				++bufp;
			}
		}
		if ( *bufp ) {
			if ( argv ) {
				*bufp = '\0';
			}
			++bufp;
		}
		if( argv && last_argc != argc ) {
			UnEscapeQuotes( argv[last_argc] );	
		}
		last_argc = argc;	
	}
	if ( argv ) {
		argv[argc] = NULL;
	}
	return(argc);
}


JNIEXPORT void JNICALL Java_com_owengine_owe_OWEJNI_init(JNIEnv *env, jclass c, jstring LibPath, jint width, jint height, jstring GameDir, jstring Cmdline)
{
    char **argv;
    int argc=0;
	jboolean iscopy;
	const char *dir = (*env)->GetStringUTFChars(
                env, GameDir, &iscopy);
    const char *arg = (*env)->GetStringUTFChars(
                env, Cmdline, &iscopy);	
	chdir(strdup(dir));
	(*env)->ReleaseStringUTFChars(env, GameDir, dir);
	argv = malloc(sizeof(char*) * 255);
	argc = ParseCommandLine(strdup(arg), argv);	
	(*env)->ReleaseStringUTFChars(env, Cmdline, arg);    
	
	const char *libpath = (*env)->GetStringUTFChars(
                env, LibPath, &iscopy);	
	loadLib(strdup(libpath));
	(*env)->ReleaseStringUTFChars(env, LibPath, libpath);    

    setCallbacks(&initAudio,&writeAudio,&setState);    
    setResolution(width, height);
    
    qmain(argc, argv);
	free(argv);
}

JNIEXPORT void JNICALL Java_com_owengine_owe_OWEJNI_drawFrame(JNIEnv *env, jclass c)
{
    onFrame();
}

JNIEXPORT void JNICALL Java_com_owengine_owe_OWEJNI_sendKeyEvent(JNIEnv *env, jclass c, jint state, jint key, jint chr)
{
    onKeyEvent(state,key,chr);
}

JNIEXPORT void JNICALL Java_com_owengine_owe_OWEJNI_sendMotionEvent(JNIEnv *env, jclass c, jfloat x, jfloat y)
{
    onMotionEvent(x, y);
}

JNIEXPORT void JNICALL Java_com_owengine_owe_OWEJNI_requestAudioData(JNIEnv *env, jclass c)
{
    onAudio();
}

JNIEXPORT void JNICALL Java_com_owengine_owe_OWEJNI_vidRestart(JNIEnv *env, jclass c)
{    
	vidRestart();
}
