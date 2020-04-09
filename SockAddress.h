#pragma once

/*
 *   Copyright (C) 2019-2020 by Thomas Early N7TAE
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <iostream>
#include <netinet/in.h>

class CSockAddress
{
public:
	CSockAddress()
	{
		Clear();
	}

	CSockAddress(const int family, const unsigned short port = 0, const char *address = NULL)
	{
		Initialize(family, port, address);
	}

	~CSockAddress() {}

	void Initialize(const int family, const uint16_t port = 0U, const char *address = NULL)
	{
		Clear();
		addr.ss_family = family;
		if (AF_INET == family) {
			auto addr4 = (struct sockaddr_in *)&addr;
			addr4->sin_port = htons(port);
			if (address) {
				if (0 == strncasecmp(address, "loc", 3))
					inet_pton(AF_INET, "127.0.0.1", &(addr4->sin_addr));
				else if (0 == strncasecmp(address, "any", 3))
					inet_pton(AF_INET, "0.0.0.0", &(addr4->sin_addr));
				else if (address) {
					if (1 > inet_pton(AF_INET, address, &(addr4->sin_addr)))
						std::cerr << "Address Initialization Error: '" << address << "' is not a valdid IPV4 address!" << std::endl;
				}
			}
			inet_ntop(AF_INET, &(addr4->sin_addr), straddr, INET6_ADDRSTRLEN);
		} else if (AF_INET6 == family) {
			auto addr6 = (struct sockaddr_in6 *)&addr;
			addr6->sin6_port = htons(port);
			if (address) {
				if (0 == strncasecmp(address, "loc", 3))
					inet_pton(AF_INET6, "::1", &(addr6->sin6_addr));
				else if (0 == strncasecmp(address, "any", 3))
					inet_pton(AF_INET6, "::", &(addr6->sin6_addr));
				else if (address) {
					if (1 > inet_pton(AF_INET6, address, &(addr6->sin6_addr)))
						std::cerr << "Address Initialization Error: '" << address << "' is not a valid IPV6 address!" << std::endl;
				}
			}
			inet_ntop(AF_INET6, &(addr6->sin6_addr), straddr, INET6_ADDRSTRLEN);
		} else
			std::cerr << "Error: Wrong address family type:" << family << " for [" << (address ? address : "NULL") << "]:" << port << std::endl;
	}

	CSockAddress &operator=(const CSockAddress &from)
	{
		Clear();
		if (AF_INET == from.addr.ss_family)
			memcpy(&addr, &from.addr, sizeof(struct sockaddr_in));
		else
			memcpy(&addr, &from.addr, sizeof(struct sockaddr_in6));
		strcpy(straddr, from.straddr);
		return *this;
	}

	bool operator==(const CSockAddress &from) const	// doesn't compare ports, only addresses and, implicitly, families
	{
		std::cout << "Does '" << straddr << "' == '" << from.straddr << "'?" << std::endl;
		const std::string addr(straddr);
		return (0 == addr.compare(from.straddr));
	}

	bool operator!=(const CSockAddress &from) const	// doesn't compare ports, only addresses and, implicitly, families
	{
		std::cout << "Does '" << straddr << "' != '" << from.straddr << "'?" << std::endl;
		const std::string addr(straddr);
		return (0 != addr.compare(from.straddr));
	}

	bool AddressIsZero() const
	{
		if (AF_INET == addr.ss_family) {
			 auto addr4 = (struct sockaddr_in *)&addr;
			return (addr4->sin_addr.s_addr == 0U);
		} else {
			auto addr6 = (struct sockaddr_in6 *)&addr;
			for (unsigned int i=0; i<16; i++) {
				if (addr6->sin6_addr.s6_addr[i])
					return false;
			}
			return true;
		}
	}

	void ClearAddress()
	{
		if (AF_INET == addr.ss_family) {
			auto addr4 = (struct sockaddr_in *)&addr;
			addr4->sin_addr.s_addr = 0U;
			strcpy(straddr, "0.0.0.0");
		} else {
			auto addr6 = (struct sockaddr_in6 *)&addr;
			memset(&(addr6->sin6_addr.s6_addr), 0, 16);
			strcpy(straddr, "::");
		}
	}

	const char *GetAddress() const
	{
		return straddr;
	}

    int GetFamily() const
    {
        return addr.ss_family;
    }

	unsigned short GetPort() const
	{
		if (AF_INET == addr.ss_family) {
			auto addr4 = (struct sockaddr_in *)&addr;
			return ntohs(addr4->sin_port);
		} else if (AF_INET6 == addr.ss_family) {
			auto addr6 = (struct sockaddr_in6 *)&addr;
			return ntohs(addr6->sin6_port);
		} else
			return 0;
	}

	void SetPort(const uint16_t newport)
	{
		if (AF_INET == addr.ss_family) {
			auto addr4 = (struct sockaddr_in *)&addr;
			addr4->sin_port = htons(newport);
		} else if (AF_INET6 == addr.ss_family) {
			auto addr6 = (struct sockaddr_in6 *)&addr;
			addr6->sin6_port = htons(newport);
		}
	}

	struct sockaddr *GetPointer()
	{
		return (struct sockaddr *)&addr;
	}

	const struct sockaddr *GetCPointer() const
	{
		return (const struct sockaddr *)&addr;
	}

	size_t GetSize() const
	{
		if (AF_INET == addr.ss_family)
			return sizeof(struct sockaddr_in);
		else
			return sizeof(struct sockaddr_in6);
	}

	void Clear()
	{
		memset(&addr, 0, sizeof(struct sockaddr_storage));
		memset(straddr, 0, INET6_ADDRSTRLEN);
	}

private:
	struct sockaddr_storage addr;
	char straddr[INET6_ADDRSTRLEN];
};
