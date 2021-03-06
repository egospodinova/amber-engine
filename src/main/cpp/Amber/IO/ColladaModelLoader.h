#ifndef COLLADAMODELLOADER_H
#define COLLADAMODELLOADER_H

#include "Amber/Utilities/Config.h"
#ifdef OpenCOLLADA_FOUND

#include "Amber/IO/IModelLoader.h"

#include <vector>

#include "Amber/Core/Entity.h"

namespace Amber
{
    namespace IO
    {
        class ColladaModelLoader : public IModelLoader
        {
            public:
                ColladaModelLoader() = default;
                virtual ~ColladaModelLoader() = default;

                virtual std::vector<Core::Entity> loadModel(const std::string &fileName) override final;

            private:
                class OCLoader;
        };
    }
}

#endif

#endif // COLLADAMODELLOADER_H
