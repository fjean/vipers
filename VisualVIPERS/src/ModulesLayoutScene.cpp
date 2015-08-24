/*
 *  Visual Video and Image Processing Environment for Real-time Systems (Visual VIPERS)
 *  Copyright (C) 2009 by Frederic Jean
 *
 *  Visual VIPERS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License,
 *  or (at your option) any later version.
 *
 *  Visual VIPERS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Visual VIPERS.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Contact:
 *  Computer Vision and Systems Laboratory
 *  Department of Electrical and Computer Engineering
 *  Universite Laval, Quebec, Canada, G1V 0A6
 *  http://vision.gel.ulaval.ca
 *
 */

#include "ModulesLayoutScene.hpp"
#include "SharedObjects.hpp"
#include "MainWindow.hpp"

#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>

#include <QDebug>

//-------------------------------------------------------------------------------

bool ModulesLayoutScene::mClickAddEnabled = false;

//-------------------------------------------------------------------------------

ModulesLayoutScene::ModulesLayoutScene()
  : QGraphicsScene(NULL)
{
  setSceneRect(QRectF(0, 0, 5000, 5000));
}

//-------------------------------------------------------------------------------

ModulesLayoutScene::~ModulesLayoutScene()
{

}

//-------------------------------------------------------------------------------

void ModulesLayoutScene::dragEnterEvent(QGraphicsSceneDragDropEvent* inEvent)
{
  if(inEvent->mimeData()->hasFormat("vipers/module-name"))
    inEvent->acceptProposedAction();
  else
    QGraphicsScene::dragEnterEvent(inEvent);
}

//-------------------------------------------------------------------------------

void ModulesLayoutScene::dragMoveEvent(QGraphicsSceneDragDropEvent* inEvent)
{

  if(inEvent->mimeData()->hasFormat("vipers/module-name"))
    inEvent->acceptProposedAction();
  else
    QGraphicsScene::dragMoveEvent(inEvent);
}

//-------------------------------------------------------------------------------

void ModulesLayoutScene::dropEvent(QGraphicsSceneDragDropEvent* inEvent)
{

  if(inEvent->mimeData()->hasFormat("vipers/module-name"))
  {
    inEvent->acceptProposedAction();
    emit moduleDropped(inEvent->mimeData()->data("vipers/module-name"), inEvent->scenePos());
  }
  else
    QGraphicsScene::dropEvent(inEvent);
}

//-------------------------------------------------------------------------------

void ModulesLayoutScene::mouseMoveEvent(QGraphicsSceneMouseEvent* inEvent)
{
  QGraphicsScene::mouseMoveEvent(inEvent);
}

//-------------------------------------------------------------------------------

void ModulesLayoutScene::mousePressEvent(QGraphicsSceneMouseEvent* inEvent)
{
  QGraphicsScene::mousePressEvent(inEvent);
  if(!inEvent->isAccepted() && getClickAddEnabled())
  {
    inEvent->accept();
    emit clickAdd(inEvent->scenePos());
  }
}

//-------------------------------------------------------------------------------
