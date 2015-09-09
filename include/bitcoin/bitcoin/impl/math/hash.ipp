/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_HASH_IPP
#define LIBBITCOIN_HASH_IPP

#include <algorithm>
#include <cstddef>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

template<size_t Size>
void scrypt(byte_array<Size> out, data_slice data, data_slice salt,
    uint64_t N, uint32_t p, uint32_t r)
{
    const auto buffer = scrypt(data, salt, N, p, r, Size);
    std::copy(buffer.begin(), buffer.end(), out.begin());
}

} // namespace libbitcoin

#endif
