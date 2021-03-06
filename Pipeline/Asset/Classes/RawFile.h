#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"


#include "Foundation/TUID.h"
#include "Foundation/Reflect/Registry.h"

namespace Helium
{
	namespace Asset
	{
		class PIPELINE_API RawFile : public AssetClass
		{
		public:

			RawFile()
			{
			}

			REFLECT_DECLARE_OBJECT( RawFile, AssetClass );

			static void PopulateComposite( Reflect::Composite& comp );
		};

		typedef Helium::SmartPtr< RawFile > RawFilePtr;
		typedef std::vector< RawFile > V_RawFile;
	}
}