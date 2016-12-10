//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
//  Copyright (C) 2016 Dusan Jocic <dusanjocic@msn.com>
//
//  This file is part of the OWEngine single player GPL Source Code.
//
//  OWengine Source Code is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OWEngine Source Code is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with OWEngine Source Code. If not, see <http://www.gnu.org/licenses/>.
//
//  In addition, the OWEngine Source Code is also subject to certain additional terms.
//  You should have received a copy of these additional terms immediately following
//  the terms and conditions of the GNU General Public License which accompanied the
//  OWEngine Source Code. If not, please request a copy in writing from id Software
//  at the address below.
//
//  If you have questions concerning this license or the applicable additional terms,
//  you may contact in writing id Software LLC, c/o ZeniMax Media Inc.,
//  Suite 120, Rockville, Maryland 20850 USA.
//
// -------------------------------------------------------------------------
//  File name:   sv_snapshot.c
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#include "server.h"

/*
=============================================================================

Delta encode a client frame onto the network channel

A normal server packet will look like:

4	sequence number (high bit set if an oversize fragment)
<optional reliable commands>
1	svc_snapshot
4	last client reliable command
4	serverTime
1	lastframe for delta compression
1	snapFlags
1	areaBytes
<areabytes>
<playerstate>
<packetentities>

=============================================================================
*/

/*
=============
SV_EmitPacketEntities

Writes a delta update of an entityState_t list to the message.
=============
*/
static void SV_EmitPacketEntities( clientSnapshot_t* from, clientSnapshot_t* to, msg_t* msg )
{
    entityState_t*   oldent, *newent;
    int oldindex, newindex;
    int oldnum, newnum;
    int from_num_entities;
    
    // generate the delta update
    if( !from )
    {
        from_num_entities = 0;
    }
    else
    {
        from_num_entities = from->num_entities;
    }
    
    newent = NULL;
    oldent = NULL;
    newindex = 0;
    oldindex = 0;
    while( newindex < to->num_entities || oldindex < from_num_entities )
    {
        if( newindex >= to->num_entities )
        {
            newnum = 9999;
        }
        else
        {
            newent = &svs.snapshotEntities[( to->first_entity + newindex ) % svs.numSnapshotEntities];
            newnum = newent->number;
        }
        
        if( oldindex >= from_num_entities )
        {
            oldnum = 9999;
        }
        else
        {
            oldent = &svs.snapshotEntities[( from->first_entity + oldindex ) % svs.numSnapshotEntities];
            oldnum = oldent->number;
        }
        
        if( newnum == oldnum )
        {
            // delta update from old position
            // because the force parm is false, this will not result
            // in any bytes being emited if the entity has not changed at all
            MSG_WriteDeltaEntity( msg, oldent, newent, false );
            oldindex++;
            newindex++;
            continue;
        }
        
        if( newnum < oldnum )
        {
            // this is a new entity, send it from the baseline
            MSG_WriteDeltaEntity( msg, &sv.svEntities[newnum].baseline, newent, true );
            newindex++;
            continue;
        }
        
        if( newnum > oldnum )
        {
            // the old entity isn't present in the new message
            MSG_WriteDeltaEntity( msg, oldent, NULL, true );
            oldindex++;
            continue;
        }
    }
    
    MSG_WriteBits( msg, ( MAX_GENTITIES - 1 ), GENTITYNUM_BITS );   // end of packetentities
}



/*
==================
SV_WriteSnapshotToClient
==================
*/
static void SV_WriteSnapshotToClient( client_t* client, msg_t* msg )
{
    clientSnapshot_t*    frame, *oldframe;
    int lastframe;
    int i;
    int snapFlags;
    
    // this is the snapshot we are creating
    frame = &client->frames[ client->netchan.outgoingSequence & PACKET_MASK ];
    
    // try to use a previous frame as the source for delta compressing the snapshot
    if( client->deltaMessage <= 0 || client->state != CS_ACTIVE )
    {
        // client is asking for a retransmit
        oldframe = NULL;
        lastframe = 0;
    }
    else if( client->netchan.outgoingSequence - client->deltaMessage
             >= ( PACKET_BACKUP - 3 ) )
    {
        // client hasn't gotten a good message through in a long time
        Com_DPrintf( "%s: Delta request from out of date packet.\n", client->name );
        oldframe = NULL;
        lastframe = 0;
    }
    else
    {
        // we have a valid snapshot to delta from
        oldframe = &client->frames[ client->deltaMessage & PACKET_MASK ];
        lastframe = client->netchan.outgoingSequence - client->deltaMessage;
        
        // the snapshot's entities may still have rolled off the buffer, though
        if( oldframe->first_entity <= svs.nextSnapshotEntities - svs.numSnapshotEntities )
        {
            Com_DPrintf( "%s: Delta request from out of date entities.\n", client->name );
            oldframe = NULL;
            lastframe = 0;
        }
    }
    
    MSG_WriteByte( msg, svc_snapshot );
    
    // NOTE, MRE: now sent at the start of every message from server to client
    // let the client know which reliable clientCommands we have received
    //MSG_WriteLong( msg, client->lastClientCommand );
    
    // send over the current server time so the client can drift
    // its view of time to try to match
    MSG_WriteLong( msg, svs.time );
    
    // what we are delta'ing from
    MSG_WriteByte( msg, lastframe );
    
    snapFlags = svs.snapFlagServerBit;
    if( client->rateDelayed )
    {
        snapFlags |= SNAPFLAG_RATE_DELAYED;
    }
    if( client->state != CS_ACTIVE )
    {
        snapFlags |= SNAPFLAG_NOT_ACTIVE;
    }
    
    MSG_WriteByte( msg, snapFlags );
    
    // send over the areabits
    MSG_WriteByte( msg, frame->areabytes );
    MSG_WriteData( msg, frame->areabits, frame->areabytes );
    
    // delta encode the playerstate
    if( oldframe )
    {
        MSG_WriteDeltaPlayerstate( msg, &oldframe->ps, &frame->ps );
    }
    else
    {
        MSG_WriteDeltaPlayerstate( msg, NULL, &frame->ps );
    }
    
    // delta encode the entities
    SV_EmitPacketEntities( oldframe, frame, msg );
    
    // padding for rate debugging
    if( sv_padPackets->integer )
    {
        for( i = 0 ; i < sv_padPackets->integer ; i++ )
        {
            MSG_WriteByte( msg, svc_nop );
        }
    }
}


/*
==================
SV_UpdateServerCommandsToClient

(re)send all server commands the client hasn't acknowledged yet
==================
*/
void SV_UpdateServerCommandsToClient( client_t* client, msg_t* msg )
{
    int i;
    
    // write any unacknowledged serverCommands
    for( i = client->reliableAcknowledge + 1 ; i <= client->reliableSequence ; i++ )
    {
        MSG_WriteByte( msg, svc_serverCommand );
        MSG_WriteLong( msg, i );
        MSG_WriteString( msg, client->reliableCommands[ i & ( MAX_RELIABLE_COMMANDS - 1 ) ] );
    }
    client->reliableSent = client->reliableSequence;
}

/*
=============================================================================

Build a client snapshot structure

=============================================================================
*/

//#define	MAX_SNAPSHOT_ENTITIES	1024
#define MAX_SNAPSHOT_ENTITIES   2048

typedef struct
{
    int numSnapshotEntities;
    int snapshotEntities[MAX_SNAPSHOT_ENTITIES];
} snapshotEntityNumbers_t;

/*
=======================
SV_QsortEntityNumbers
=======================
*/
static int SV_QsortEntityNumbers( const void* a, const void* b )
{
    int* ea, *eb;
    
    ea = ( int* )a;
    eb = ( int* )b;
    
    if( *ea == *eb )
    {
        Com_Error( ERR_DROP, "SV_QsortEntityStates: duplicated entity" );
    }
    
    if( *ea < *eb )
    {
        return -1;
    }
    
    return 1;
}


/*
===============
SV_AddEntToSnapshot
===============
*/
static void SV_AddEntToSnapshot( svEntity_t* svEnt, sharedEntity_t* gEnt, snapshotEntityNumbers_t* eNums )
{
    // if we have already added this entity to this snapshot, don't add again
    if( svEnt->snapshotCounter == sv.snapshotCounter )
    {
        return;
    }
    svEnt->snapshotCounter = sv.snapshotCounter;
    
    // if we are full, silently discard entities
    if( eNums->numSnapshotEntities == MAX_SNAPSHOT_ENTITIES )
    {
        return;
    }
    
    eNums->snapshotEntities[ eNums->numSnapshotEntities ] = gEnt->s.number;
    eNums->numSnapshotEntities++;
}

/*
===============
SV_AddEntitiesVisibleFromPoint
===============
*/
static void SV_AddEntitiesVisibleFromPoint( vec3_t origin, clientSnapshot_t* frame,
//									snapshotEntityNumbers_t *eNums, bool portal, clientSnapshot_t *oldframe, bool localClient ) {
//									snapshotEntityNumbers_t *eNums, bool portal ) {
        snapshotEntityNumbers_t* eNums, bool portal, bool localClient )
{
    int e, i;
    sharedEntity_t* ent, *playerEnt;
    svEntity_t*  svEnt;
    int l;
    int clientarea, clientcluster;
    int leafnum;
    int c_fullsend;
    byte*    clientpvs;
    byte*    bitvector;
    
    // during an error shutdown message we may need to transmit
    // the shutdown message after the server has shutdown, so
    // specfically check for it
    if( !sv.state )
    {
        return;
    }
    
    leafnum = collisionModelManager->PointLeafnum( origin );
    clientarea = collisionModelManager->LeafArea( leafnum );
    clientcluster = collisionModelManager->LeafCluster( leafnum );
    
    // calculate the visible areas
    frame->areabytes = collisionModelManager->WriteAreaBits( frame->areabits, clientarea );
    
    clientpvs = collisionModelManager->ClusterPVS( clientcluster );
    
    c_fullsend = 0;
    
    playerEnt = SV_GentityNum( frame->ps.clientNum );
    
    for( e = 0 ; e < sv.num_entities ; e++ )
    {
        ent = SV_GentityNum( e );
        
        // never send entities that aren't linked in
        if( !ent->r.linked )
        {
            continue;
        }
        
        if( ent->s.number != e )
        {
            Com_DPrintf( "FIXING ENT->S.NUMBER!!!\n" );
            ent->s.number = e;
        }
        
        // entities can be flagged to explicitly not be sent to the client
        if( ent->r.svFlags & SVF_NOCLIENT )
        {
            continue;
        }
        
        // entities can be flagged to be sent to only one client
        if( ent->r.svFlags & SVF_SINGLECLIENT )
        {
            if( ent->r.singleClient != frame->ps.clientNum )
            {
                continue;
            }
        }
        // entities can be flagged to be sent to everyone but one client
        if( ent->r.svFlags & SVF_NOTSINGLECLIENT )
        {
            if( ent->r.singleClient == frame->ps.clientNum )
            {
                continue;
            }
        }
        
        svEnt = SV_SvEntityForGentity( ent );
        
        // don't double add an entity through portals
        if( svEnt->snapshotCounter == sv.snapshotCounter )
        {
            continue;
        }
        
        // if this client is viewing from a camera, only add ents visible from portal ents
        if( ( playerEnt->s.eFlags & EF_VIEWING_CAMERA ) && !portal )
        {
            if( ent->r.svFlags & SVF_PORTAL )
            {
                SV_AddEntToSnapshot( svEnt, ent, eNums );
//				SV_AddEntitiesVisibleFromPoint( ent->s.origin2, frame, eNums, true, oldframe, localClient );
                SV_AddEntitiesVisibleFromPoint( ent->s.origin2, frame, eNums, true, localClient );
            }
            continue;
        }
        
        // broadcast entities are always sent
        if( ent->r.svFlags & SVF_BROADCAST )
        {
            SV_AddEntToSnapshot( svEnt, ent, eNums );
            continue;
        }
        
        // ignore if not touching a PV leaf
        // check area
        if( !collisionModelManager->AreasConnected( clientarea, svEnt->areanum ) )
        {
            // doors can legally straddle two areas, so
            // we may need to check another one
            if( !collisionModelManager->AreasConnected( clientarea, svEnt->areanum2 ) )
            {
                goto notVisible;    // blocked by a door
            }
        }
        
        bitvector = clientpvs;
        
        // check individual leafs
        if( !svEnt->numClusters )
        {
            goto notVisible;
        }
        l = 0;
        for( i = 0 ; i < svEnt->numClusters ; i++ )
        {
            l = svEnt->clusternums[i];
            if( bitvector[l >> 3] & ( 1 << ( l & 7 ) ) )
            {
                break;
            }
        }
        
        // if we haven't found it to be visible,
        // check overflow clusters that coudln't be stored
        if( i == svEnt->numClusters )
        {
            if( svEnt->lastCluster )
            {
                for( ; l <= svEnt->lastCluster ; l++ )
                {
                    if( bitvector[l >> 3] & ( 1 << ( l & 7 ) ) )
                    {
                        break;
                    }
                }
                if( l == svEnt->lastCluster )
                {
                    goto notVisible;    // not visible
                }
            }
            else
            {
                goto notVisible;
            }
        }
        
        //----(SA) added "visibility dummies"
        if( ent->r.svFlags & SVF_VISDUMMY )
        {
            sharedEntity_t* ment = 0;
            
            //find master;
            ment = SV_GentityNum( ent->s.otherEntityNum );
            
            if( ment )
            {
                svEntity_t* master = 0;
                master = SV_SvEntityForGentity( ment );
                
                if( master->snapshotCounter == sv.snapshotCounter || !ment->r.linked )
                {
                    goto notVisible;
                    //continue;
                }
                
                SV_AddEntToSnapshot( master, ment, eNums );
            }
            goto notVisible;
            //continue;	// master needs to be added, but not this dummy ent
        }
        //----(SA) end
        else if( ent->r.svFlags & SVF_VISDUMMY_MULTIPLE )
        {
            {
                int h;
                sharedEntity_t* ment = 0;
                svEntity_t* master = 0;
                
                for( h = 0; h < sv.num_entities; h++ )
                {
                    ment = SV_GentityNum( h );
                    
                    if( ment == ent )
                    {
                        continue;
                    }
                    
                    if( ment )
                    {
                        master = SV_SvEntityForGentity( ment );
                    }
                    else
                    {
                        continue;
                    }
                    
                    if( !( ment->r.linked ) )
                    {
                        continue;
                    }
                    
                    if( ment->s.number != h )
                    {
                        Com_DPrintf( "FIXING vis dummy multiple ment->S.NUMBER!!!\n" );
                        ment->s.number = h;
                    }
                    
                    if( ment->r.svFlags & SVF_NOCLIENT )
                    {
                        continue;
                    }
                    
                    if( master->snapshotCounter == sv.snapshotCounter )
                    {
                        continue;
                    }
                    
                    if( ment->s.otherEntityNum == ent->s.number )
                    {
                        SV_AddEntToSnapshot( master, ment, eNums );
                    }
                }
                goto notVisible;
            }
        }
        
        // add it
        SV_AddEntToSnapshot( svEnt, ent, eNums );
        
        // if its a portal entity, add everything visible from its camera position
        if( ent->r.svFlags & SVF_PORTAL )
        {
//			SV_AddEntitiesVisibleFromPoint( ent->s.origin2, frame, eNums, true, oldframe, localClient );
            SV_AddEntitiesVisibleFromPoint( ent->s.origin2, frame, eNums, true, localClient );
        }
        
        continue;
        
notVisible:

        // Ridah, if this entity has changed events, then send it regardless of whether we can see it or not
        // DHM - Nerve :: not in multiplayer please
        if( sv_gametype->integer == GT_SINGLE_PLAYER && localClient )
        {
            if( ent->r.eventTime == svs.time )
            {
                ent->s.eFlags |= EF_NODRAW;     // don't draw, just process event
                SV_AddEntToSnapshot( svEnt, ent, eNums );
            }
            else if( ent->s.eType == ET_PLAYER )
            {
                // keep players around if they are alive and active (so sounds dont get messed up)
                if( !( ent->s.eFlags & EF_DEAD ) )
                {
                    ent->s.eFlags |= EF_NODRAW;     // don't draw, just process events and sounds
                    SV_AddEntToSnapshot( svEnt, ent, eNums );
                }
            }
        }
        
    }
}

/*
=============
SV_BuildClientSnapshot

Decides which entities are going to be visible to the client, and
copies off the playerstate and areabits.

This properly handles multiple recursive portals, but the render
currently doesn't.

For viewing through other player's eyes, clent can be something other than client->gentity
=============
*/
static void SV_BuildClientSnapshot( client_t* client )
{
    vec3_t org;
//	clientSnapshot_t			*frame, *oldframe;
    clientSnapshot_t*            frame;
    snapshotEntityNumbers_t entityNumbers;
    int i;
    sharedEntity_t*              ent;
    entityState_t*               state;
    svEntity_t*                  svEnt;
    sharedEntity_t*              clent;
    int clientNum;
    playerState_t*               ps;
    
    // bump the counter used to prevent double adding
    sv.snapshotCounter++;
    
    // this is the frame we are creating
    frame = &client->frames[ client->netchan.outgoingSequence & PACKET_MASK ];
    
//	// try to use a previous frame as the source for delta compressing the snapshot
//	if ( client->deltaMessage <= 0 || client->state != CS_ACTIVE ) {
//		// client is asking for a retransmit
//		oldframe = NULL;
//	} else if ( client->netchan.outgoingSequence - client->deltaMessage
//		>= (PACKET_BACKUP - 3) ) {
//		// client hasn't gotten a good message through in a long time
//		Com_DPrintf ("%s: Delta request from out of date packet.\n", client->name);
//		oldframe = NULL;
//	} else {
//		// we have a valid snapshot to delta from
//		oldframe = &client->frames[ client->deltaMessage & PACKET_MASK ];
//
//		// the snapshot's entities may still have rolled off the buffer, though
//		if ( oldframe->first_entity <= svs.nextSnapshotEntities - svs.numSnapshotEntities ) {
//			Com_DPrintf ("%s: Delta request from out of date entities.\n", client->name);
//			oldframe = NULL;
//		}
//	}

    // clear everything in this snapshot
    entityNumbers.numSnapshotEntities = 0;
    memset( frame->areabits, 0, sizeof( frame->areabits ) );
    
    frame->num_entities = 0;
    
    clent = client->gentity;
    if( !clent || client->state == CS_ZOMBIE )
    {
        return;
    }
    
    // grab the current playerState_t
    ps = SV_GameClientNum( client - svs.clients );
    frame->ps = *ps;
    
    // never send client's own entity, because it can
    // be regenerated from the playerstate
    clientNum = frame->ps.clientNum;
    if( clientNum < 0 || clientNum >= MAX_GENTITIES )
    {
        Com_Error( ERR_DROP, "SV_SvEntityForGentity: bad gEnt" );
    }
    svEnt = &sv.svEntities[ clientNum ];
    
    svEnt->snapshotCounter = sv.snapshotCounter;
    
    // find the client's viewpoint
    VectorCopy( ps->origin, org );
    org[2] += ps->viewheight;
    
//----(SA)	added for 'lean'
    // need to account for lean, so areaportal doors draw properly
    if( frame->ps.leanf != 0 )
    {
        vec3_t right, v3ViewAngles;
        VectorCopy( ps->viewangles, v3ViewAngles );
        v3ViewAngles[2] += frame->ps.leanf / 2.0f;
        AngleVectors( v3ViewAngles, NULL, right, NULL );
        VectorMA( org, frame->ps.leanf, right, org );
    }
//----(SA)	end

    // add all the entities directly visible to the eye, which
    // may include portal entities that merge other viewpoints
    SV_AddEntitiesVisibleFromPoint( org, frame, &entityNumbers, false, client->netchan.remoteAddress.type == NA_LOOPBACK );
//	SV_AddEntitiesVisibleFromPoint( org, frame, &entityNumbers, false, oldframe, client->netchan.remoteAddress.type == NA_LOOPBACK );

    // if there were portals visible, there may be out of order entities
    // in the list which will need to be resorted for the delta compression
    // to work correctly.  This also catches the error condition
    // of an entity being included twice.
    qsort( entityNumbers.snapshotEntities, entityNumbers.numSnapshotEntities,
           sizeof( entityNumbers.snapshotEntities[0] ), SV_QsortEntityNumbers );
           
    // now that all viewpoint's areabits have been OR'd together, invert
    // all of them to make it a mask vector, which is what the renderer wants
    for( i = 0 ; i < MAX_MAP_AREA_BYTES / 4 ; i++ )
    {
        ( ( int* )frame->areabits )[i] = ( ( int* )frame->areabits )[i] ^ -1;
    }
    
    // copy the entity states out
    frame->num_entities = 0;
    frame->first_entity = svs.nextSnapshotEntities;
    for( i = 0 ; i < entityNumbers.numSnapshotEntities ; i++ )
    {
        ent = SV_GentityNum( entityNumbers.snapshotEntities[i] );
        state = &svs.snapshotEntities[svs.nextSnapshotEntities % svs.numSnapshotEntities];
        *state = ent->s;
        svs.nextSnapshotEntities++;
        // this should never hit, map should always be restarted first in SV_Frame
        if( svs.nextSnapshotEntities >= 0x7FFFFFFE )
        {
            Com_Error( ERR_FATAL, "svs.nextSnapshotEntities wrapped" );
        }
        frame->num_entities++;
    }
}


/*
====================
SV_RateMsec

Return the number of msec a given size message is supposed
to take to clear, based on the current rate
====================
*/
#define HEADER_RATE_BYTES   48      // include our header, IP header, and some overhead
static int SV_RateMsec( client_t* client, int messageSize )
{
    int rate;
    int rateMsec;
    int maxRate;
    
    // individual messages will never be larger than fragment size
    if( messageSize > 1500 )
    {
        messageSize = 1500;
    }
    // low watermark for sv_maxRate, never 0 < sv_maxRate < 1000 (0 is no limitation)
    if( sv_maxRate->integer && sv_maxRate->integer < 1000 )
    {
        Cvar_Set( "sv_MaxRate", "1000" );
    }
    rate = client->rate;
    // work on the appropriate max rate (client or download)
    if( !*client->downloadName )
    {
        maxRate = sv_maxRate->integer;
    }
    else
    {
        maxRate = sv_dl_maxRate->integer;
    }
    if( maxRate )
    {
        if( maxRate < rate )
        {
            rate = maxRate;
        }
    }
    rateMsec = ( messageSize + HEADER_RATE_BYTES ) * 1000 / rate;
    
    return rateMsec;
}

/*
=======================
SV_SendMessageToClient

Called by SV_SendClientSnapshot and SV_SendClientGameState
=======================
*/
void SV_SendMessageToClient( msg_t* msg, client_t* client )
{
    int rateMsec;
    
    // record information about the message
    client->frames[client->netchan.outgoingSequence & PACKET_MASK].messageSize = msg->cursize;
    client->frames[client->netchan.outgoingSequence & PACKET_MASK].messageSent = svs.time;
    client->frames[client->netchan.outgoingSequence & PACKET_MASK].messageAcked = -1;
    
    // send the datagram
    SV_Netchan_Transmit( client, msg );
    
    // set nextSnapshotTime based on rate and requested number of updates
    
    // local clients get snapshots every frame
    // added sv_lanForceRate check
    if( client->netchan.remoteAddress.type == NA_LOOPBACK || ( sv_lanForceRate->integer && Sys_IsLANAddress( client->netchan.remoteAddress ) ) )
    {
        client->nextSnapshotTime = svs.time - 1;
        return;
    }
    
    // normal rate / snapshotMsec calculation
    rateMsec = SV_RateMsec( client, msg->cursize );
    
    // during a download, ignore the snapshotMsec
    // the update server on steroids, with this disabled and sv_fps 60, the download can reach 30 kb/s
    // on a regular server, we will still top at 20 kb/s because of sv_fps 20
    if( !*client->downloadName && rateMsec < client->snapshotMsec )
    {
        // never send more packets than this, no matter what the rate is at
        rateMsec = client->snapshotMsec;
        client->rateDelayed = false;
    }
    else
    {
        client->rateDelayed = true;
    }
    
    client->nextSnapshotTime = svs.time + rateMsec;
    
    // don't pile up empty snapshots while connecting
    if( client->state != CS_ACTIVE )
    {
        // a gigantic connection message may have already put the nextSnapshotTime
        // more than a second away, so don't shorten it
        // do shorten if client is downloading
        if( !*client->downloadName && client->nextSnapshotTime < svs.time + 1000 )
        {
            client->nextSnapshotTime = svs.time + 1000;
        }
    }
}


/*
=======================
SV_SendClientSnapshot

Also called by SV_FinalMessage

=======================
*/
void SV_SendClientSnapshot( client_t* client )
{
    byte msg_buf[MAX_MSGLEN];
    msg_t msg;
    
    //RF, AI don't need snapshots built
    if( client->gentity && client->gentity->r.svFlags & SVF_CASTAI )
    {
        return;
    }
    
    // build the snapshot
    SV_BuildClientSnapshot( client );
    
    // bots need to have their snapshots build, but
    // the query them directly without needing to be sent
    if( client->gentity && client->gentity->r.svFlags & SVF_BOT )
    {
        return;
    }
    
    MSG_Init( &msg, msg_buf, sizeof( msg_buf ) );
    msg.allowoverflow = true;
    
    // NOTE, MRE: all server->client messages now acknowledge
    // let the client know which reliable clientCommands we have received
    MSG_WriteLong( &msg, client->lastClientCommand );
    
    // (re)send any reliable server commands
    SV_UpdateServerCommandsToClient( client, &msg );
    
    // send over all the relevant entityState_t
    // and the playerState_t
    SV_WriteSnapshotToClient( client, &msg );
    
    // Add any download data if the client is downloading
    SV_WriteDownloadToClient( client, &msg );
    
    // check for overflow
    if( msg.overflowed )
    {
        Com_Printf( "WARNING: msg overflowed for %s\n", client->name );
        MSG_Clear( &msg );
    }
    
    SV_SendMessageToClient( &msg, client );
    sv.bpsTotalBytes += msg.cursize;            // NERVE - SMF - net debugging
    sv.ubpsTotalBytes += msg.uncompsize / 8;    // NERVE - SMF - net debugging
}

/*
=======================
SV_SendClientMessages
=======================
*/
void SV_SendClientMessages( void )
{
    int i;
    client_t*    c;
    int numclients = 0;         // NERVE - SMF - net debugging
    
    sv.bpsTotalBytes = 0;       // NERVE - SMF - net debugging
    sv.ubpsTotalBytes = 0;      // NERVE - SMF - net debugging
    
    // send a message to each connected client
    for( i = 0, c = svs.clients; i < sv_maxclients->integer; i++, c++ )
    {
        if( !c->state )
        {
            continue;       // not connected
        }
        
        if( svs.time < c->nextSnapshotTime )
        {
            continue;       // not time yet
        }
        
        numclients++;       // NERVE - SMF - net debugging
        
        // send additional message fragments if the last message
        // was too large to send at once
        if( c->netchan.unsentFragments )
        {
            c->nextSnapshotTime = svs.time +
                                  SV_RateMsec( c, c->netchan.unsentLength - c->netchan.unsentFragmentStart );
            SV_Netchan_TransmitNextFragment( c );
            continue;
        }
        
        // generate and send a new message
        SV_SendClientSnapshot( c );
    }
    
    // net debugging
    if( sv_showAverageBPS->integer && numclients > 0 )
    {
        float ave = 0, uave = 0;
        
        for( i = 0; i < MAX_BPS_WINDOW - 1; i++ )
        {
            sv.bpsWindow[i] = sv.bpsWindow[i + 1];
            ave += sv.bpsWindow[i];
            
            sv.ubpsWindow[i] = sv.ubpsWindow[i + 1];
            uave += sv.ubpsWindow[i];
        }
        
        sv.bpsWindow[MAX_BPS_WINDOW - 1] = sv.bpsTotalBytes;
        ave += sv.bpsTotalBytes;
        
        sv.ubpsWindow[MAX_BPS_WINDOW - 1] = sv.ubpsTotalBytes;
        uave += sv.ubpsTotalBytes;
        
        if( sv.bpsTotalBytes >= sv.bpsMaxBytes )
        {
            sv.bpsMaxBytes = sv.bpsTotalBytes;
        }
        
        if( sv.ubpsTotalBytes >= sv.ubpsMaxBytes )
        {
            sv.ubpsMaxBytes = sv.ubpsTotalBytes;
        }
        
        sv.bpsWindowSteps++;
        
        if( sv.bpsWindowSteps >= MAX_BPS_WINDOW )
        {
            float comp_ratio;
            
            sv.bpsWindowSteps = 0;
            
            ave = ( ave / ( float )MAX_BPS_WINDOW );
            uave = ( uave / ( float )MAX_BPS_WINDOW );
            
            comp_ratio = ( 1 - ave / uave ) * 100.f;
            sv.ucompAve += comp_ratio;
            sv.ucompNum++;
            
            Com_DPrintf( "bpspc(%2.0f) bps(%2.0f) pk(%i) ubps(%2.0f) upk(%i) cr(%2.2f) acr(%2.2f)\n",
                         ave / ( float )numclients, ave, sv.bpsMaxBytes, uave, sv.ubpsMaxBytes, comp_ratio, sv.ucompAve / sv.ucompNum );
        }
    }
}

