#pragma once

//#include"core/reference.h"

#include "utils/persistence/persistence.h"

namespace Grafkit
{

    // TODO Get rid of this later
    class Object : public Serializable
    {
        
    public:
        template <class Archive>
        void Serialize(Archive& ar)
        {
            // Nothing to do, get rid of this later
        }

    };

}
