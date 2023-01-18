#include "outlineTreeModel.h"

#include "render/SceneGraph.h"

#include "render/camera.h"
#include "render/model.h"
#include "render/Light.h"

#include "animation/actoranimation.h"

#include <qdebug.h>
#include "models/Curve.h"

using namespace Grafkit;
using namespace Idogep;

// ------------------------------------------------------------------------------------------

DemoItem::DemoItem(const GkDemo::DemoRef& demo, TreeItem* parentItem) : TreeItem(parentItem)
, m_demo(demo)
{
    setIcon(QPixmap(":/icons/demo.png").scaled(16, 16));
    m_itemData << QString::fromStdString("Demo");
}

GkDemo::DemoRef DemoItem::GetDemo() const { return m_demo; }

// ------------------------------------------------------------------------------------------

SceneGraphItem::SceneGraphItem(SceneGraphRef& scenegraph, TreeItem* parentItem)
    : m_scenegraph(scenegraph)
{
    setIcon(QPixmap(":/icons/scene.png").scaled(16, 16));
    m_itemData << QString::fromStdString(scenegraph->GetName());
}

SceneGraphRef SceneGraphItem::GetSceneGraph() const { return m_scenegraph; }


// ------------------------------------------------------------------------------------------

ActorItem::ActorItem(ActorRef& actor, TreeItem* parentItem)
{
    setIcon(QPixmap(":/icons/node.png").scaled(16, 16));
    m_itemData << QString::fromStdString(actor->GetName());
    //SetParentName(actor->GetName());
}

// ------------------------------------------------------------------------------------------

EntityItem::EntityItem(Entity3DRef entity, TreeItem* parentItem)
    : m_entity(entity)
{
    setIcon(QPixmap(":/icons/model.png").scaled(16, 16));
    m_itemData << QString::fromStdString(entity->GetName());
}

ModelItem::ModelItem(ModelRef model, TreeItem* parentItem)
    : EntityItem(dynamic_cast<Model*>(model.Get()), parentItem)
{
    setIcon(QPixmap(":/icons/model.png").scaled(16, 16));
    m_itemData << QString::fromStdString(model->GetName());
}

CameraItem::CameraItem(CameraRef camera, TreeItem* parentItem)
    : EntityItem(dynamic_cast<Entity3D*>(camera.Get()), parentItem)
{
    setIcon(QPixmap(":/icons/camera.png").scaled(16, 16));
    m_itemData << QString::fromStdString(camera->GetName());
}

LightItem::LightItem(LightRef light, TreeItem* parentItem)
    : EntityItem(dynamic_cast<Entity3D*>(light.Get()), parentItem)
{
    setIcon(QPixmap(":/icons/light.png").scaled(16, 16));
    m_itemData << QString::fromStdString(light->GetName());
}


// ------------------------------------------------------------------------------------------

#include "scene/scene.h"
#include "demo.h"

DemoModel::DemoModel(const GkDemo::DemoRef& demo) : m_demo(demo) {
}

DemoModel::~DemoModel() {
}

QStringList DemoModel::Header()
{
    return QStringList() << "Node"; //<< "V" << "W" << "Lock";
}

void DemoModel::BuildAnimations(const HasAnimationsRole * const &container)
{
    assert(container);
    for (size_t i = 0; i < container->GetAnimationCount(); i++)
    {
        Animation* animation = container->GetAnimation(i);

        // --- 
        Ref<ActorAnimation> actorAnimation = dynamic_cast<ActorAnimation*>(animation);
        if (actorAnimation)
        {
            Actor* actor = actorAnimation->GetActor();
            auto it = m_actorMap.find(actor);
            if (it != m_actorMap.end()) { it->second->SetAnimation(new AnimationWrapper(actorAnimation, actor->GetName())); }
            // TODO remove this later on
#if 0
            qDebug() << "Animation set OutlineItem " << QString::fromStdString(actorAnimation->GetName()) << " Ptr:" << actorAnimation.Get();

            // --- 
            {
                for (size_t k = 0; k < animation->GetTrackCount(); ++k)
                {
                    Ref<Animation::Track> track = animation->GetTrack(k);
                    qDebug() << "- Track"<< QString::fromStdString(track->GetName()) << "Ptr:" << track.Get();

                    for (size_t l = 0; l < track->GetChannelCount(); ++l)
                    {
                        Ref<Animation::Channel> channel = track->GetChannel(l);
                        qDebug() << "-- Channel" << QString::fromStdString(channel->GetName()) << "Ptr:" << channel.Get();
                    }
                }
            }
            // --- 
#endif
        }

        // masikra ugyanez:
        //EntityAnimation * entityAnimation = dynamic_cast<EntityAnimation*>(animation);
        // if (...) ... ;
    }
}

void DemoModel::BuildSceneGraph(TreeItem* parentItem, SceneGraphRef sceneGraph) {

    if (sceneGraph.Invalid())
    {
        qDebug() << "scenegraph == nullptr!";
        return;
    }

    TreeItem* item = new SceneGraphItem(sceneGraph, parentItem);
    parentItem->addChild(item);

    // add actors 
    BuildActor(item, sceneGraph->GetRootNode());
}

void DemoModel::Build(TreeItem* parentItem)
{
    if (m_demo.Invalid())
    {
        qDebug() << "demo == nullptr!";
        return;
    }

    TreeItem* item = new DemoItem(m_demo, parentItem);
    parentItem->addChild(item);

    size_t count = m_demo->GetSceneCount();
    for (size_t i = 0; i < count; i++)
    {
        SceneResRef sceneRes = m_demo->GetScene(i);
        if (sceneRes && *sceneRes)
        {
            BuildSceneGraph(parentItem, (*sceneRes)->GetSceneGraph());
            BuildAnimations(dynamic_cast<HasAnimationsRole*>((*sceneRes).Get()));
        }
    }

    // TODO any other stuff?
    for (size_t i = 0; i < m_demo->GetAnimationCount(); i++)
    {
        Animation* animation = m_demo->GetAnimation(i);
        ItemHasAnimationsRole* animationItem = dynamic_cast<ItemHasAnimationsRole*>(item);
        animationItem->SetAnimation(new AnimationWrapper(animation, "Demo"));
        break; // TODO: muliple animation
    }
}

void DemoModel::BuildActor(TreeItem* parentItem, ActorRef actor, int maxDepth)
{
    ActorItem* item = new ActorItem(actor, parentItem);
    parentItem->addChild(item);

    for (int i = 0; i < actor->GetChildrenCount(); i++) { BuildActor(item, actor->GetChild(i), maxDepth - 1); }

    for (int i = 0; i < actor->GetEntityCount(); i++) { BuildEntity(item, actor->GetEntity(i)); }

    m_actorMap.insert_or_assign(actor, item);
}

void DemoModel::BuildEntity(TreeItem* parentItem, Entity3DRef entity)
{
    QList<QVariant> rowData;

    EntityItem* item = nullptr;

    const CameraRef camera = dynamic_cast<Camera*>(entity.Get());
    const ModelRef model = dynamic_cast<Model*>(entity.Get());
    const LightRef light = dynamic_cast<Light*>(entity.Get());

    if (camera) { item = new CameraItem(camera, parentItem); }
    else if (light)
    {
        /*BuildCamera(item, camera);*/
        item = new LightItem(light, parentItem);
    }
    else
        if (model) { item = new ModelItem(model, parentItem); }

    if (item)
    {
        parentItem->addChild(item);
        m_entityMap.insert_or_assign(entity, item);
    }
}
