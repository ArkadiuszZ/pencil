/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "layermanager.h"

#include "object.h"
#include "editor.h"

#include "layersound.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"


LayerManager::LayerManager( QObject* pParent ) : BaseManager( pParent )
{
}

LayerManager::~LayerManager()
{
}

bool LayerManager::init()
{
    lastCameraLayer = 0;
    return true;
}

Status LayerManager::load( Object* o )
{
    connect( o, &Object::layerChanged, this, &LayerManager::layerUpdated );

    mCurrentLayerIndex = o->data()->getCurrentLayer();
    lastCameraLayer = 0;

    emit layerCountChanged(o->getLayerCount());

    return Status::OK;
}

Status LayerManager::save( Object* o )
{
	o->data()->setCurrentLayer( mCurrentLayerIndex );
	return Status::OK;
}

int LayerManager::getLastCameraLayer()
{
    return lastCameraLayer;
}

Layer* LayerManager::currentLayer()
{
    return currentLayer( 0 );
}

Layer* LayerManager::currentLayer( int incr )
{
    Q_ASSERT( editor()->object() != NULL );

    return editor()->object()->getLayer( mCurrentLayerIndex + incr );
}

Layer* LayerManager::getLayer( int index )
{
    Q_ASSERT( editor()->object() != NULL );
    
    return editor()->object()->getLayer( index );
}

int LayerManager::currentLayerIndex()
{
    return mCurrentLayerIndex;
}

void LayerManager::setCurrentLayer( int layerIndex )
{
    Object* o = editor()->object();
    
    if ( layerIndex >= o->getLayerCount() )
    {
        Q_ASSERT( false );
        return;
    }

    if ( mCurrentLayerIndex != layerIndex )
    {
        mCurrentLayerIndex = layerIndex;
        Q_EMIT currentLayerChanged( mCurrentLayerIndex );
    }

    if ( editor()->object() )
    {
        if ( editor()->object()->getLayer( layerIndex )->type() == Layer::CAMERA )
        {
            lastCameraLayer = layerIndex;
        }
    }
}

void LayerManager::setCurrentLayer( Layer* layer )
{
    Object* o = editor()->object();

    for ( int i = 0; i < o->getLayerCount(); ++i )
    {
        if ( layer == o->getLayer( i ) )
        {
            setCurrentLayer( i );
            return;
        }
    }
}

void LayerManager::gotoNextLayer()
{
    if ( mCurrentLayerIndex < editor()->object()->getLayerCount() - 1 )
    {
        mCurrentLayerIndex += 1;
		Q_EMIT currentLayerChanged( mCurrentLayerIndex );
    }
}

void LayerManager::gotoPreviouslayer()
{
    if ( mCurrentLayerIndex > 0 )
    {
        mCurrentLayerIndex -= 1;
		Q_EMIT currentLayerChanged( mCurrentLayerIndex );
    }
}

LayerBitmap* LayerManager::createBitmapLayer( const QString& strLayerName )
{
    LayerBitmap* layer = editor()->object()->addNewBitmapLayer();
    layer->setName( strLayerName );
    
    Q_EMIT layerCountChanged( count() );
    
    return layer;
}

LayerVector* LayerManager::createVectorLayer( const QString& strLayerName )
{
    LayerVector* layer = editor()->object()->addNewVectorLayer();
    layer->setName( strLayerName );
    
    Q_EMIT layerCountChanged( count() );
    
    return layer;
}

LayerCamera* LayerManager::createCameraLayer( const QString& strLayerName )
{
    LayerCamera* layer = editor()->object()->addNewCameraLayer();
    layer->setName( strLayerName );
    
    Q_EMIT layerCountChanged( count() );
    
    return layer;
}

LayerSound* LayerManager::createSoundLayer( const QString& strLayerName )
{
    LayerSound* layer = editor()->object()->addNewSoundLayer();
    layer->setName( strLayerName );
    
    Q_EMIT layerCountChanged( count() );

    return layer;
}

int LayerManager::LastFrameAtFrame( int frameIndex )
{
    Object* pObj = editor()->object();
    for ( int i = frameIndex; i >= 0; i -= 1 )
    {
        for ( int layerIndex = 0; layerIndex < pObj->getLayerCount(); ++layerIndex )
        {
            auto pLayer = pObj->getLayer( layerIndex );
            if ( pLayer->keyExists( i ) )
            {
                return i;
            }
        }
    }
    return -1;
}

int LayerManager::firstKeyFrameIndex()
{
    int minPosition = INT_MAX;

    Object* pObj = editor()->object();
    for ( int i = 0; i < pObj->getLayerCount(); ++i )
    {
        Layer* pLayer = pObj->getLayer( i );

        int position = pLayer->firstKeyFramePosition();
        if ( position < minPosition )
        {
            minPosition = position;
        }
    }
    return minPosition;
}

int LayerManager::lastKeyFrameIndex()
{
    int maxPosition = 0;

    for ( int i = 0; i < editor()->object()->getLayerCount(); ++i )
    {
        Layer* pLayer = editor()->object()->getLayer( i );

        int position = pLayer->getMaxKeyFramePosition();
        if ( position > maxPosition )
        {
            maxPosition = position;
        }
    }
    return maxPosition;
}

int LayerManager::count()
{
    return editor()->object()->getLayerCount();
}

bool LayerManager::deleteCurrentLayer()
{
    // FIXME: 
    if ( currentLayer()->type() == Layer::CAMERA )
    {
        return false;
    }

    editor()->object()->deleteLayer( currentLayerIndex() );

    if ( currentLayerIndex() == editor()->object()->getLayerCount() )
    {
        setCurrentLayer( currentLayerIndex() - 1 );
    }

    Q_EMIT layerCountChanged( count() );

    return true;
}

/**
 * @brief LayerManager::projectLength
 * @return int: position of the last key frame in the timeline + its length
 */
int LayerManager::projectLength()
{
    int maxFrame = -1;

    Object* pObject = editor()->object();
    for ( int i = 0; i < pObject->getLayerCount(); i++ )
    {
        int lastFramePos = pObject->getLayer( i )->getMaxKeyFramePosition();
        int prevFramePos = pObject->getLayer( i )->getPreviousKeyFramePosition(lastFramePos);

        if (pObject->getLayer(i)->type() == Layer::SOUND) {
            KeyFrame* keyFrame = pObject->getLayer( i )->getKeyFrameWhichCovers(lastFramePos);
            KeyFrame* prevKFrame = pObject->getLayer( i )->getKeyFrameWhichCovers(prevFramePos);

            if ( keyFrame != nullptr && prevKFrame != nullptr ) {
                if ( keyFrame->length() >= 1 &&
                     keyFrame->pos() + keyFrame->length() > maxFrame )
                {
                    maxFrame = lastFramePos + keyFrame->length();
                }

                // in cases where the previous frame length is longer and further away
                // than the last frame length
                if ( prevKFrame->pos() + prevKFrame->length() > keyFrame->pos() + keyFrame->length() )
                {
                    maxFrame = prevFramePos + prevKFrame->length();
                }
            }
        }
        else if ( lastFramePos > maxFrame )
        {
            maxFrame = lastFramePos;
        }
    }
    return maxFrame;
}

void LayerManager::layerUpdated(int layerId)
{
    emit currentLayerChanged(layerId);
}
