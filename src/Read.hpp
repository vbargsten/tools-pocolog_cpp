/* 
 * Copyright (c) 2005-2006 LAAS/CNRS <openrobots@laas.fr>
 *	Sylvain Joyeux <sylvain.joyeux@m4x.org>
 *
 * All rights reserved.
 *
 * Redistribution and use  in source  and binary  forms,  with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice,  this list of  conditions and the following disclaimer in
 *      the  documentation  and/or  other   materials provided  with  the
 *      distribution.
 *
 * THIS  SOFTWARE IS PROVIDED BY  THE  COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND  ANY  EXPRESS OR IMPLIED  WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR  PURPOSE ARE DISCLAIMED. IN  NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR      CONTRIBUTORS  BE LIABLE FOR   ANY    DIRECT, INDIRECT,
 * INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE   OF THIS SOFTWARE, EVEN   IF ADVISED OF   THE POSSIBILITY OF SUCH
 * DAMAGE.
 */


#ifndef POCOLOG_CPP_READ_HPP
#define POCOLOG_CPP_READ_HPP

#include "pocolog_cpp/Format.hpp"

#include <string>
#include <vector>
#include <iosfwd>
#include <typelib/value_ops.hh>

#include <boost/noncopyable.hpp>

namespace Typelib
{
    class Registry;
    class Type;
}

namespace pocolog_cpp
{
    class Stream;
    class DataStream;
    class DataInputIterator;

    class Input
    {
    public:
        static const size_t npos    = static_cast<size_t>(-1);
        static const size_t nstream = static_cast<size_t>(-1);

    private:
        std::istream* m_input;

        typedef std::vector<Stream*> Streams;
        Streams m_streams;

        void readBlockData(std::vector<uint8_t>& buffer, size_t size);
        bool readStreamDeclaration(const std::vector<uint8_t>& buffer, size_t stream_idx, size_t pos, size_t size);
        BlockHeader skip(const BlockHeader& header);

    public:
        Input();
        ~Input();

        void init(std::istream& input);

        size_t      size() const;
        Stream& operator [] (size_t index) const;

	/// returns the data stream with the given name or NULL if there is no such stream
	DataStream& getDataStream( const std::string& name ) const;

        /// Helper functions for input iterators
        static void readBlockData(std::istream& stream, std::vector<uint8_t>& buffer, size_t size);
        static BlockHeader skip(std::istream& stream, const BlockHeader& header);
    };


    class Stream
        : boost::noncopyable
    {
        friend class Input;

        std::istream& m_input;
        StreamType    m_type;
        size_t        m_index;

        size_t m_size,
               m_datasize,
               m_begin,
               m_end;

    protected:
        Stream(std::istream& input, StreamType type = UnknownStreamType, size_t index = Input::nstream)
            : m_input(input)
            , m_type(type)
            , m_index(index)
            , m_size(0), m_datasize(0)
            , m_begin(Input::npos)
            , m_end(Input::npos) {}

        virtual BlockHeader newDataBlock(BlockHeader header) = 0;
        /** Subclasses call that in their newBlock implementation
         * to update the begin and end positions */
        void newBlockAt(size_t pos, size_t size)
        {
            if (m_begin == Input::npos)
                m_begin = m_end = pos;
            else if (m_end == Input::npos || m_end < pos)
                m_end = pos;

            m_datasize += m_datasize;
            ++m_size;
        }

        std::istream& getInputStream() { return m_input; }
    public:
        virtual ~Stream() {}
        
        bool   isValid() const        { return m_type != UnknownStreamType; }

        StreamType getType() const    { return m_type; }
        size_t getIndex() const       { return m_index; }
        size_t getBeginPos() const    { return m_begin; }
        size_t getEndPos() const      { return m_end; }
        size_t getSize() const        { return m_size; }
        size_t getDataSize() const    { return m_datasize; }
    };

    class DataStream : public Stream
    {
        friend class Input;
        std::string m_name;
        std::string m_type_name;
        
        Typelib::Type*       m_type;
        Typelib::Registry*   m_registry;

        BlockHeader m_firstheader;
        base::Time m_begin, m_end;
       
    protected:
        DataStream
                ( std::istream& input, size_t index
                , const std::string& name, const std::string& type_name
                , Typelib::Registry* registry );

    public:
        ~DataStream();
        std::string getName() const;
        std::string getTypeName() const;
        Typelib::Type const*     getType() const;
        Typelib::Registry const* getTypeRegistry() const;
        
        DataInputIterator begin();
        DataInputIterator end();

        base::Time   getBeginTime() const;
        base::Time   getEndTime() const;

        virtual BlockHeader newDataBlock(BlockHeader header);
    };

    class ControlStream : public Stream
    {
        friend class Input;
    protected:
        ControlStream(std::istream& input, size_t index);

    public:
        virtual BlockHeader newDataBlock(BlockHeader header);
    };


   

    class DataInputIterator
    {
        friend class DataStream;

        Typelib::Type const* m_sample_type;
        std::istream*        m_input;
        size_t               m_index;
        size_t               m_pos;

        BlockHeader          m_block_header;
        SampleHeader         m_sample_header;
        std::vector<uint8_t> m_buffer;

    private:
        DataInputIterator
            ( Typelib::Type const* sample_type
            , std::istream& input, size_t stream_index
            , size_t first_block, const BlockHeader& first_header);
        void failed();
        void readCurBlock(size_t size);

    public:
	DataInputIterator();
        ~DataInputIterator();

        base::Time getRealtime() const;
        base::Time getTimestamp() const;

        bool isValid() const;

        size_t getPos() const { return m_pos; }
        
        size_t getDataSize() const;
        const uint8_t* getData() const;
        template<typename T>
        T getData() const
        {
            T out;
            getData<T>(out);
            return out;
        }

        template<typename T>
        void getData(T& out) const
        {
            std::vector<uint8_t> buffer(m_buffer.begin() + SAMPLE_HEADER_SIZE, m_buffer.end());
            Typelib::Value v(&out, *m_sample_type);
            Typelib::load(v, buffer);
        }

        DataInputIterator& operator += (size_t offset);
        DataInputIterator  operator +  (size_t offset) const;
        DataInputIterator& operator ++();

        bool operator == (const DataInputIterator& with) const;
        bool operator != (const DataInputIterator& with) const
        { return ! ((*this) == with); }
    };





    class FileException : public std::exception 
    {
        size_t m_pos;
    public:
        FileException(size_t pos = Input::npos) 
            : m_pos(pos) {}
        ~FileException() throw() {}

        virtual void output(std::ostream& display) throw();
    };
    struct Truncated : public FileException 
    { virtual void output(std::ostream& display) throw(); };
    struct BadMagic : public FileException
    { virtual void output(std::ostream& display) throw(); };

    class BadStream : public FileException
    {
        const size_t m_index;
    public:
        BadStream(size_t index, size_t pos) 
            : FileException(pos), m_index(index) {}
        virtual void output(std::ostream& display) throw();
    };

    class BadBlockType : public FileException
    {
        const size_t m_type;
    public:
        BadBlockType(size_t type, size_t pos) 
            : FileException(pos), m_type(type) {}
        virtual void output(std::ostream& display) throw();
    };
    class DataBlockException : public FileException
    {
        const BlockType m_type;

    public:
        DataBlockException(BlockType type, size_t pos)
            : FileException(pos), m_type(type) {}
        ~DataBlockException() throw() {}

        virtual void output(std::ostream& display) throw();
    };
    struct BadDataSize : public DataBlockException
    {
        BadDataSize(BlockType type, size_t pos)
            : DataBlockException(type, pos) {}

        virtual void output(std::ostream& display) throw();
    };
    struct DataSizeMismatch : public DataBlockException
    {
        DataSizeMismatch(BlockType type, size_t pos)
            : DataBlockException(type, pos) {}

        virtual void output(std::ostream& display) throw();
    };
    struct BadStreamType : public FileException
    {
        BadStreamType(size_t pos)
            : FileException(pos) {}

        virtual void output(std::ostream& display) throw();
    };
    struct NoSuchStream : public FileException
    { 
	const std::string name;

	NoSuchStream(const std::string& name) : name(name) {};
        ~NoSuchStream() throw() {}
	virtual void output(std::ostream& display) throw(); 
    };
}

#endif

