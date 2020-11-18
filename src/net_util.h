// See the file "COPYING" in the main distribution directory for copyright.

#pragma once

#include "zeek-config.h"

// Define first.
typedef enum
{
	TRANSPORT_UNKNOWN,
	TRANSPORT_TCP,
	TRANSPORT_UDP,
	TRANSPORT_ICMP,
} TransportProto;

typedef enum
{
	IPv4,
	IPv6
} IPFamily;

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <sys/types.h>
#ifdef HAVE_LINUX
#define __FAVOR_BSD
#endif
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include "zeek/util.h"

#ifdef HAVE_NETINET_IP6_H
#include <netinet/ip6.h>

#ifndef HAVE_IP6_OPT
struct ip6_opt
	{
	uint8_t ip6o_type;
	uint8_t ip6o_len;
	};
#endif // HAVE_IP6_OPT

#ifndef HAVE_IP6_EXT
struct ip6_ext
	{
	uint8_t ip6e_nxt;
	uint8_t ip6e_len;
	};
#endif // HAVE_IP6_EXT

#else

struct ip6_hdr
	{
	union {
		struct ip6_hdrctl
			{
			uint32_t ip6_un1_flow; /* 4 bits version, 8 bits TC, 20 bits
			                          flow-ID */
			uint16_t ip6_un1_plen; /* payload length */
			uint8_t ip6_un1_nxt; /* next header */
			uint8_t ip6_un1_hlim; /* hop limit */
			} ip6_un1;
		uint8_t ip6_un2_vfc; /* 4 bits version, top 4 bits tclass */
		} ip6_ctlun;
	struct in6_addr ip6_src; /* source address */
	struct in6_addr ip6_dst; /* destination address */
	};

#define ip6_vfc ip6_ctlun.ip6_un2_vfc
#define ip6_flow ip6_ctlun.ip6_un1.ip6_un1_flow
#define ip6_plen ip6_ctlun.ip6_un1.ip6_un1_plen
#define ip6_nxt ip6_ctlun.ip6_un1.ip6_un1_nxt
#define ip6_hlim ip6_ctlun.ip6_un1.ip6_un1_hlim
#define ip6_hops ip6_ctlun.ip6_un1.ip6_un1_hlim

struct ip6_opt
	{
	uint8_t ip6o_type;
	uint8_t ip6o_len;
	};

struct ip6_ext
	{
	uint8_t ip6e_nxt;
	uint8_t ip6e_len;
	};

struct ip6_frag
	{
	uint8_t ip6f_nxt; /* next header */
	uint8_t ip6f_reserved; /* reserved field */
	uint16_t ip6f_offlg; /* offset, reserved, and flag */
	uint32_t ip6f_ident; /* identification */
	};

struct ip6_hbh
	{
	uint8_t ip6h_nxt; /* next header */
	uint8_t ip6h_len; /* length in units of 8 octets */
	/* followed by options */
	};

struct ip6_dest
	{
	uint8_t ip6d_nxt; /* next header */
	uint8_t ip6d_len; /* length in units of 8 octets */
	/* followed by options */
	};

struct ip6_rthdr
	{
	uint8_t ip6r_nxt; /* next header */
	uint8_t ip6r_len; /* length in units of 8 octets */
	uint8_t ip6r_type; /* routing type */
	uint8_t ip6r_segleft; /* segments left */
	/* followed by routing type specific data */
	};
#endif // HAVE_NETINET_IP6_H

// For Solaris.
#if ! defined(TCPOPT_WINDOW) && defined(TCPOPT_WSCALE)
#define TCPOPT_WINDOW TCPOPT_WSCALE
#endif

#if ! defined(TCPOPT_TIMESTAMP) && defined(TCPOPT_TSTAMP)
#define TCPOPT_TIMESTAMP TCPOPT_TSTAMP
#endif

ZEEK_FORWARD_DECLARE_NAMESPACED(IPAddr, zeek);
ZEEK_FORWARD_DECLARE_NAMESPACED(IP_Hdr, zeek);

namespace zeek
	{

namespace detail
	{

struct checksum_block
	{
	const uint8_t* block;
	int len;
	};

struct ipv4_pseudo_hdr
	{
	in_addr src;
	in_addr dst;
	uint8_t zero;
	uint8_t next_proto;
	uint16_t len;
	};

struct ipv6_pseudo_hdr
	{
	in6_addr src;
	in6_addr dst;
	uint32_t len;
	uint8_t zero[3];
	uint8_t next_proto;
	};

extern uint16_t in_cksum(const checksum_block* blocks, int num_blocks);

inline uint16_t in_cksum(const uint8_t* data, int len)
	{
	checksum_block cb {data, len};
	return in_cksum(&cb, 1);
	}

extern uint16_t ip4_in_cksum(const IPAddr& src, const IPAddr& dst, uint8_t next_proto,
                             const uint8_t* data, int len);

extern uint16_t ip6_in_cksum(const IPAddr& src, const IPAddr& dst, uint8_t next_proto,
                             const uint8_t* data, int len);

inline uint16_t ip_in_cksum(bool is_ipv4, const IPAddr& src, const IPAddr& dst, uint8_t next_proto,
                            const uint8_t* data, int len)
	{
	if ( is_ipv4 )
		return ip4_in_cksum(src, dst, next_proto, data, len);
	return ip6_in_cksum(src, dst, next_proto, data, len);
	}

	} // namespace zeek::detail

// Returns the ones-complement checksum of a chunk of 'b' bytes.
extern int ones_complement_checksum(const void* p, int b, uint32_t sum);

extern int ones_complement_checksum(const IPAddr& a, uint32_t sum);

extern int icmp6_checksum(const struct icmp* icmpp, const IP_Hdr* ip, int len);
extern int icmp_checksum(const struct icmp* icmpp, int len);

#ifdef ENABLE_MOBILE_IPV6
extern int mobility_header_checksum(const IP_Hdr* ip);
#endif

// True if sequence # a is between b and c (b <= a <= c).  It must be true
// that b <= c in the sequence space.
inline bool seq_between(uint32_t a, uint32_t b, uint32_t c)
	{
	if ( b <= c )
		return a >= b && a <= c;
	else
		return a >= b || a <= c;
	}

// Returns a - b, adjusted for sequence wraparound.
inline int32_t seq_delta(uint32_t a, uint32_t b)
	{
	return a - b;
	}

// Returns 'A', 'B', 'C' or 'D'
extern char addr_to_class(uint32_t addr);

extern const char* fmt_conn_id(const IPAddr& src_addr, uint32_t src_port, const IPAddr& dst_addr,
                               uint32_t dst_port);
extern const char* fmt_conn_id(const uint32_t* src_addr, uint32_t src_port,
                               const uint32_t* dst_addr, uint32_t dst_port);

/**
 * Given a MAC address, formats it in hex as 00:de:ad:be:ef.
 * Supports both EUI-48 and EUI-64. If it's neither, returns
 * an empty string.
 *
 * @param m EUI-48 or EUI-64 MAC address to format, as a char array
 * @param len Number of bytes valid starting at *n*. This must be at
 *            least 8 for a valid address.
 * @return A string of the formatted MAC. Passes ownership to caller.
 */
extern std::string fmt_mac(const unsigned char* m, int len);

// Read 4 bytes from data and return in network order.
extern uint32_t extract_uint32(const u_char* data);

// Endian conversions for double.
// This is certainly not a very clean solution but should work on the
// major platforms. Alternativly, we could use a string format or the
// XDR library.

#ifdef WORDS_BIGENDIAN

inline double ntohd(double d)
	{
	return d;
	}
inline double htond(double d)
	{
	return d;
	}

#ifndef HAVE_BYTEORDER_64
inline uint64_t ntohll(uint64_t i)
	{
	return i;
	}
inline uint64_t htonll(uint64_t i)
	{
	return i;
	}
#endif

#else

inline double ntohd(double d)
	{
	assert(sizeof(d) == 8);

	double tmp;
	char* src = (char*)&d;
	char* dst = (char*)&tmp;

	dst[0] = src[7];
	dst[1] = src[6];
	dst[2] = src[5];
	dst[3] = src[4];
	dst[4] = src[3];
	dst[5] = src[2];
	dst[6] = src[1];
	dst[7] = src[0];

	return tmp;
	}

inline double htond(double d)
	{
	return ntohd(d);
	}

#ifndef HAVE_BYTEORDER_64
inline uint64_t ntohll(uint64_t i)
	{
	u_char c;
	union {
		uint64_t i;
		u_char c[8];
		} x;

	x.i = i;
	c = x.c[0];
	x.c[0] = x.c[7];
	x.c[7] = c;
	c = x.c[1];
	x.c[1] = x.c[6];
	x.c[6] = c;
	c = x.c[2];
	x.c[2] = x.c[5];
	x.c[5] = c;
	c = x.c[3];
	x.c[3] = x.c[4];
	x.c[4] = c;
	return x.i;
	}

inline uint64_t htonll(uint64_t i)
	{
	return ntohll(i);
	}
#endif

#endif

	} // namespace zeek

constexpr auto seq_between [[deprecated("Remove in v4.1. Use zeek::seq_between.")]] =
	zeek::seq_between;
constexpr auto seq_delta [[deprecated("Remove in v4.1. Use zeek::seq_delta.")]] = zeek::seq_delta;
constexpr auto icmp6_checksum [[deprecated("Remove in v4.1. Use zeek::icmp6_checksum.")]] =
	zeek::icmp6_checksum;
constexpr auto icmp_checksum [[deprecated("Remove in v4.1. Use zeek::icmp_checksum.")]] =
	zeek::icmp_checksum;

[[deprecated("Remove in v4.1. Use zeek::ones_complement_checksum.")]] inline int
ones_complement_checksum(const void* p, int b, uint32_t sum)
	{
	return zeek::ones_complement_checksum(p, b, sum);
	}
[[deprecated("Remove in v4.1. Use zeek::ones_complement_checksum.")]] inline int
ones_complement_checksum(const zeek::IPAddr& a, uint32_t sum)
	{
	return zeek::ones_complement_checksum(a, sum);
	}

#ifdef ENABLE_MOBILE_IPV6
constexpr auto mobility_header_checksum
	[[deprecated("Remove in v4.1. Use zeek::mobility_header_checksum.")]] =
		zeek::mobility_header_checksum;
#endif

constexpr auto addr_to_class [[deprecated("Remove in v4.1. Use zeek::addr_to_class.")]] =
	zeek::addr_to_class;

[[deprecated("Remove in v4.1. Use zeek::fmt_conn_id.")]] inline const char*
fmt_conn_id(const zeek::IPAddr& src_addr, uint32_t src_port, const zeek::IPAddr& dst_addr,
            uint32_t dst_port)
	{
	return zeek::fmt_conn_id(src_addr, src_port, dst_addr, dst_port);
	}
[[deprecated("Remove in v4.1. Use zeek::fmt_conn_id.")]] inline const char*
fmt_conn_id(const uint32_t* src_addr, uint32_t src_port, const uint32_t* dst_addr,
            uint32_t dst_port)
	{
	return zeek::fmt_conn_id(src_addr, src_port, dst_addr, dst_port);
	}

constexpr auto fmt_mac [[deprecated("Remove in v4.1. Use zeek::fmt_mac.")]] = zeek::fmt_mac;
constexpr auto extract_uint32 [[deprecated("Remove in v4.1. Use zeek::extract_uint32.")]] =
	zeek::extract_uint32;

constexpr auto ntohd [[deprecated("Remove in v4.1. Use zeek::ntohd.")]] = zeek::ntohd;
constexpr auto htond [[deprecated("Remove in v4.1. Use zeek::htond.")]] = zeek::htond;

#ifndef HAVE_BYTEORDER_64
constexpr auto ntohll [[deprecated("Remove in v4.1. Use zeek::ntohll.")]] = zeek::ntohll;
constexpr auto htonll [[deprecated("Remove in v4.1. Use zeek::htonll.")]] = zeek::htonll;
#endif
