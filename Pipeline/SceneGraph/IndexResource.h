#pragma once

#include "Resource.h" 

namespace Helium
{
    namespace SceneGraph
    {

        class IndexResource : public Resource
        {
        public:
            static Profile::MemoryPoolHandle s_MemoryPool;
            IDirect3DIndexBuffer9*           m_Buffer; 

            IndexResource( ResourceTracker* tracker ); 

            IDirect3DIndexBuffer9* GetBuffer() const
            {
                return m_Buffer; 
            }

            virtual uint8_t* Lock() HELIUM_OVERRIDE; 
            virtual void Unlock() HELIUM_OVERRIDE; 
            virtual bool SetState() const HELIUM_OVERRIDE; 

        protected:
            virtual bool Allocate() HELIUM_OVERRIDE; 
            virtual void Release() HELIUM_OVERRIDE; 
        };

        typedef Helium::StrongPtr<IndexResource> IndexResourcePtr;

    }
}