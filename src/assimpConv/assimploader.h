#pragma once

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/matrix4x4.h"

#include "assimp/vector2.h"
#include "assimp/vector3.h"

#include "utils/exceptions.h"

#include "utils/asset.h"
#include "utils/resource.h"
#include "utils/ResourceBuilder.h"
			
#include "render/Scene.h"
#include "render/model.h"
#include "render/texture.h"

struct aiNode;
struct aiMaterial;

namespace Grafkit {

	/*
		A huge dirty Importer class for the engine 
	*/

	class AssimpLoader
	{
	public:
		AssimpLoader();
		AssimpLoader(void* data, size_t length);
		~AssimpLoader();

		// convert to left-handed
		void SetLHFlag(bool islh);


		SceneResRef Load();
		void AppendAssimp(const void* data, size_t length, SceneRef inscene);

	protected:

		// TODO: store these in Scene and get them from there

		struct {
			std::vector<MaterialRef>		materials;
			std::vector<ModelRef>			models;
			std::vector<CameraRef>			cameras;
			std::vector<LightRef>			lights;
			std::map<std::string, ActorRef> actors;
		} m_resources;


	private:		
		void AssimpLoadMaterials(SceneRef &outScene);
		void AssimpLoadMeshes(SceneRef &outScene);
		void AssimpLoadCameras(SceneRef &outScene);
		void AssimpLoadLights(SceneRef &outScene);
		void AssimpBuildScenegraph(SceneRef &outScene);
		void AssimpLoadAnimations(SceneRef &outScene);

		void AssimpParseScenegraphNode(aiNode* ai_node, ActorRef &actor_node, int maxdepth = 1024);

	private:
		static Grafkit::Matrix ai4x4MatrixToFWMatrix(aiMatrix4x4 * m);
		static TextureResRef assimpTexture(enum aiTextureType source, aiMaterial* material, int subnode, Grafkit::IResourceManager * const & resman);
		static inline aiVector3D crossProduct(aiVector3D a, aiVector3D b);
		static inline void swap_vertices(aiVector3D *vertices, char order[], char polarity[]);

	private:
		const void* m_data;
		size_t m_length;

		Assimp::Importer importer;
		const aiScene *aiscene;

		bool m_is_lh;
	};

}

// ================================================================================================================================================================
// Assimp helpers
// ================================================================================================================================================================

// aiVector3D to float3
#define ASSIMP_V3D_F3(SRC, DST)\
{\
	(DST).x = (SRC).x, (DST).y = (SRC).y, (DST).z = (SRC).z;\
}

#define ASSIMP_V3D_F3_SET(SRC, DST)\
(DST(((float)(SRC).x), ((float)(SRC).y), ((float)(SRC).z)))


// aiVector3D to float4
#define ASSIMP_V3D_F4(SRC, DST, W)\
{\
	(DST).x = (SRC).x, (DST).y = (SRC).y, (DST).z = (SRC).z, (DST).w = (W);\
}

// aiVector4D to float4
#define ASSIMP_V4D_F4(SRC, DST)\
{\
	(DST).x = (SRC).x, (DST).y = (SRC).y, (DST).z = (SRC).z, (DST).w = (SRC).w;\
}

#define ASSIMP_V3D_F4_SET(SRC, DST)\
(DST(((float)(SRC).x), ((float)(SRC).y), ((float)(SRC).z)))


#define ASSIMP_COLOR_F4(SRC, DST)\
{\
	(DST).x = (SRC).r, (DST).y = (SRC).g, (DST).z = (SRC).b, (DST).z = 1.0f; \
}

// ezeket mashogy nem lehet megoldnai, mert a matkey is egy makro
#define ASSIMPMATERIALKEY_F4(SRC, _AI_ENUM, OUT)\
{\
	aiColor3D ac; float4& fv = (OUT); \
	(SRC)->Get(_AI_ENUM, ac); \
	fv.x = ac.r, fv.y = ac.g, fv.z = ac.b, fv.w = 1.0; \
}

#define ASSIMPMATERIALKEY_FLOAT(SRC, _AI_ENUM, OUT) \
{\
	float &scalar = (OUT);\
	(SRC)->Get(_AI_ENUM, scalar);\
}

// blender workaround miatt kell
// vertex order nem oke, (x y z) -> (x -z y)
#if 0
#define assimp_vertices(SRC, DST, W)\
{\
	(DST).x = (SRC).x, (DST).y = -(SRC).z, (DST).z = (SRC).y, (DST).w = (W);\
}
#else 
#define assimp_vertices(SRC, DST, W) ASSIMP_V3D_F4(SRC, DST, W)
#endif


// exceptions
DEFINE_EXCEPTION(AssimpParseException, 4000, "Error while parsing file via ASSIMP");
