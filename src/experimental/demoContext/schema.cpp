#include <fstream>

#include "schema.h"
#include "demo.h"
#include "json.hpp"

#include "core/logger.h"

#include "core/asset.h"
#include "utils/asset/AssetFactory.h"
#include "resource/ResourceManager.h"

#include "render/shader.h"
#include "resource/loaders/ShaderLoader.h"

#include "render/mesh.h"
#include "render/model.h"
#include "render/material.h"
#include "render/texture.h"

#include "render/light.h"
#include "render/camera.h"

#include "render/actor.h"
#include "render/SceneGraph.h"

#include "scene/scene.h"
//#include "resource/loaders/AnimationLoader.h"

#include "core/Music.h"
#include "resource/loaders/MusicBassLoader.h"

#include "builtin_data/cube.h"
#include "json_fwd.hpp"
#include "animation/actoranimation.h"
#include "json.hpp"

//#include "DemoAnimationLoader.h"
//#include "MeshLoader.h"
#include "context.h"
#include "resource/loaders/TextureLoader.h"

//#define ANIMATION_ROOT "animation/"

using namespace GkDemo;
using namespace Grafkit;
using namespace FWdebugExceptions;

using json = nlohmann::json;

SchemaBuilder::SchemaBuilder() : m_demo(nullptr)
    , m_inited(false) {
}

SchemaBuilder::~SchemaBuilder() {
}


/* ======================================================================
 * Loading stuff
 * ======================================================================
*/

void SchemaBuilder::LoadFromAsset(const IAssetRef& asset, IResourceManager* resourceManager) const {
    assert(0);
}

DemoRef SchemaBuilder::GetDemo() const { return m_demo; }
