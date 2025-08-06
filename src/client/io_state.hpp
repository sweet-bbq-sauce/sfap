/*!
 *  \file
 *  \brief Defines the IOState class representing basic I/O stream state flags.
 *
 *  \copyright Copyright (c) 2025 Wiktor Sołtys
 *
 *  \cond
 *  MIT License
 * 
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  \endcond
 */


#pragma once


#include <array>

#include <sfap.hpp>


namespace sfap {

    namespace utils {


        /*!
         *  \class IOState
         *  \brief Encapsulates basic I/O stream state flags and provides (de)serialization.
         *
         *  This class holds the three most common I/O stream states (`fail`, `bad`, `eof`)
         *  and allows compact encoding into a single byte. It is intended for use in
         *  stream-like components of the SFAP protocol stack.
         */
        class IOState {
            
            public:

                /*!
                 *  \brief Constructs a default IOState with all flags cleared.
                 */
                IOState() noexcept;

                /*!
                 *  \brief Constructs an IOState from a raw byte representation.
                 *  \param serialized Byte containing packed stream state flags.
                 */
                IOState( byte_t serialized ) noexcept;

                /*!
                 *  \brief Constructs an IOState with specified state flags.
                 *  \param _fail Whether the failbit is set.
                 *  \param _bad  Whether the badbit is set.
                 *  \param _eof  Whether the eofbit is set.
                 */
                IOState( bool _fail, bool _bad, bool _eof ) noexcept;

                /*!
                 *  \brief Checks whether the failbit is set.
                 *  \return True if the failbit is set.
                 */
                bool fail() const noexcept;

                /*!
                 *  \brief Checks whether the badbit is set.
                 *  \return True if the badbit is set.
                 */
                bool bad() const noexcept;

                /*!
                 *  \brief Checks whether the eofbit is set.
                 *  \return True if the eofbit is set.
                 */
                bool eof() const noexcept;

                /*!
                 *  \brief Serializes the current state flags into a byte.
                 *  \return A byte representing the packed state flags.
                 */
                byte_t serialize() const noexcept;

                /*!
                 *  \brief Restores state flags from a serialized byte.
                 *  \param raw Byte containing the packed flags.
                 */
                void deserialize( byte_t raw ) noexcept;


            private:

                bool _fail_flag;
                bool _bad_flag;
                bool _eof_flag;

        };


    }
    
}