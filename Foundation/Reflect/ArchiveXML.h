#pragma once

#include "Foundation/XMLDocument.h"
#include "Foundation/Reflect/Indent.h"
#include "Foundation/Reflect/Archive.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API ArchiveXML : public Archive
        {
        public: 
            static const uint32_t CURRENT_VERSION; 

        private:
            friend class Archive;

            // File format version
            uint32_t m_Version;

            // File size
            std::streamoff m_Size;

            // Skip flag
            bool m_Skip;

            // The xml data
            XMLDocument m_Document;

            // The current element
            XMLDocument::Iterator m_Iterator;

            // The stream for element bodies
            TCharStream* m_Body;

            // The stream for the file
            TCharStreamPtr m_Stream;

            // Indentation helper
            Indent<tchar_t> m_Indent;

        public:
            ArchiveXML( const Path& path, ByteOrder byteOrder = Helium::PlatformByteOrder );
            ~ArchiveXML();

        private:
            ArchiveXML();

        public:
            // Stream access
            TCharStream& GetStream()
            {
				TCharStream* stream = m_Mode == ArchiveModes::Read ? m_Body : m_Stream;
				HELIUM_ASSERT( stream );
				return *stream;
            }

        protected:
            // The type
            virtual ArchiveType GetType() const
            {
                return ArchiveTypes::XML;
            }

            virtual void Open( bool write = false ) HELIUM_OVERRIDE;
            void OpenStream(TCharStream* stream, bool write = false );
            virtual void Close() HELIUM_OVERRIDE; 

            // Begins parsing the InputStream
            virtual void Read() HELIUM_OVERRIDE;

            // Write to the OutputStream
            virtual void Write() HELIUM_OVERRIDE;

        public:
            // Access indentation
            Indent<tchar_t>& GetIndent()
            {
                return m_Indent;
            }

        public:
            void SerializeInstance( Object* object );
            void SerializeInstance( void* structure, const Structure* type );

        protected:
            void SerializeInstance( Object* object, const tchar_t* fieldName );
            void SerializeInstance( void* structure, const Structure* type, const tchar_t* fieldName );

		public:
			void SerializeFields( Object* object );
            void SerializeFields( void* structure, const Structure* type );
            void SerializeArray( const std::vector< ObjectPtr >& elements, uint32_t flags = 0 );
            void SerializeArray( const DynArray< ObjectPtr >& elements, uint32_t flags = 0 );

        protected:
            template< typename ConstIteratorType >
            void SerializeArray( ConstIteratorType begin, ConstIteratorType end, uint32_t flags );

        public:
            // Deserialize
            void DeserializeInstance( ObjectPtr& object );
            void DeserializeInstance( void* structure, const Structure* type );
            void DeserializeFields( Object* object );
            void DeserializeFields( void* object, const Structure* type );
            void DeserializeArray( std::vector< ObjectPtr >& elements, uint32_t flags = 0 );
            void DeserializeArray( DynArray< ObjectPtr >& elements, uint32_t flags = 0 );

        protected:
            // Helpers
            template< typename ArrayPusher >
            void DeserializeArray( ArrayPusher& push, uint32_t flags );
            ObjectPtr Allocate();

        public:
            // Reading and writing single object from string data
            static void       ToString( Object* object, tstring& xml );
            static ObjectPtr  FromString( const tstring& xml, const Class* searchClass = NULL );

            // Reading and writing multiple elements from string data
            static void       ToString( const std::vector< ObjectPtr >& elements, tstring& xml );
            static void       FromString( const tstring& xml, std::vector< ObjectPtr >& elements );
        };
    }
}