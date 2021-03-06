// See the file "COPYING" in the main distribution directory for copyright.

#pragma once

#include "zeek/IntrusivePtr.h"
#include "zeek/ID.h"
#include "zeek/Type.h"

ZEEK_FORWARD_DECLARE_NAMESPACED(EventHandlerPtr, zeek);
ZEEK_FORWARD_DECLARE_NAMESPACED(StringVal, zeek);
ZEEK_FORWARD_DECLARE_NAMESPACED(TableVal, zeek);
ZEEK_FORWARD_DECLARE_NAMESPACED(ListVal, zeek);
ZEEK_FORWARD_DECLARE_NAMESPACED(FuncType, zeek);
ZEEK_FORWARD_DECLARE_NAMESPACED(Stmt, zeek::detail);
ZEEK_FORWARD_DECLARE_NAMESPACED(Expr, zeek::detail);
ZEEK_FORWARD_DECLARE_NAMESPACED(Scope, zeek::detail);

namespace zeek::detail {

using StmtPtr = IntrusivePtr<Stmt>;

enum DeclType { VAR_REGULAR, VAR_CONST, VAR_REDEF, VAR_OPTION, };

extern void add_global(const IDPtr& id, TypePtr t, InitClass c, ExprPtr init,
                       std::unique_ptr<std::vector<AttrPtr>> attr, DeclType dt);

extern StmtPtr add_local(IDPtr id, TypePtr t, InitClass c, ExprPtr init,
												 std::unique_ptr<std::vector<AttrPtr>> attr, DeclType dt);

extern ExprPtr add_and_assign_local(IDPtr id, ExprPtr init, ValPtr val = nullptr);

extern void add_type(ID* id, TypePtr t,
                     std::unique_ptr<std::vector<AttrPtr>> attr);

extern void begin_func(IDPtr id, const char* module_name, FunctionFlavor flavor,
                       bool is_redef, FuncTypePtr t,
                       std::unique_ptr<std::vector<AttrPtr>> attrs = nullptr);

extern void end_func(StmtPtr body);

// Gather all IDs referenced inside a body that aren't part of a given scope.
extern IDPList gather_outer_ids(Scope* scope, Stmt* body);

} // namespace zeek::detail

using decl_type [[deprecated("Remove in v4.1. Use zeek::detail::DeclType.")]] = zeek::detail::DeclType;
constexpr auto VAR_REGULAR [[deprecated("Remove in v4.1. Use zeek::detail::VAR_REGULAR.")]] = zeek::detail::VAR_REGULAR;
constexpr auto VAR_CONST [[deprecated("Remove in v4.1. Use zeek::detail::VAR_CONST.")]] = zeek::detail::VAR_CONST;
constexpr auto VAR_REDEF [[deprecated("Remove in v4.1. Use zeek::detail::VAR_REDEF.")]] = zeek::detail::VAR_REDEF;
constexpr auto VAR_OPTION [[deprecated("Remove in v4.1. Use zeek::detail::VAR_OPTION.")]] = zeek::detail::VAR_OPTION;

constexpr auto add_global [[deprecated("Remove in v4.1. Use zeek::detail::add_global.")]] = zeek::detail::add_global;
constexpr auto add_local [[deprecated("Remove in v4.1. Use zeek::detail::add_local.")]] = zeek::detail::add_local;
constexpr auto add_and_assign_local [[deprecated("Remove in v4.1. Use zeek::detail::add_and_assign_local.")]] = zeek::detail::add_and_assign_local;
constexpr auto add_type [[deprecated("Remove in v4.1. Use zeek::detail::add_type.")]] = zeek::detail::add_type;
constexpr auto begin_func [[deprecated("Remove in v4.1. Use zeek::detail::begin_func.")]] = zeek::detail::begin_func;
constexpr auto end_func [[deprecated("Remove in v4.1. Use zeek::detail::end_func.")]] = zeek::detail::end_func;
constexpr auto gather_outer_ids [[deprecated("Remove in v4.1. Use zeek::detail::gather_outer_ids.")]] = zeek::detail::gather_outer_ids;

[[deprecated("Remove in v4.1.  Use zeek::id::find_val().")]]
extern zeek::Val* internal_val(const char* name);

[[deprecated("Remove in v4.1.  Use zeek::id::find_const().")]]
extern zeek::Val* internal_const_val(const char* name); // internal error if not const

[[deprecated("Remove in v4.1.  Use zeek::id::find() or zeek::id::find_val().")]]
extern zeek::Val* opt_internal_val(const char* name);	// returns nil if not defined

[[deprecated("Remove in v4.1.  Use zeek::id::find() or zeek::id::find_val().")]]
extern double opt_internal_double(const char* name);

[[deprecated("Remove in v4.1.  Use zeek::id::find() or zeek::id::find_val().")]]
extern bro_int_t opt_internal_int(const char* name);

[[deprecated("Remove in v4.1.  Use zeek::id::find() or zeek::id::find_val().")]]
extern bro_uint_t opt_internal_unsigned(const char* name);

[[deprecated("Remove in v4.1.  Use zeek::id::find() or zeek::id::find_val().")]]
extern zeek::StringVal* opt_internal_string(const char* name);

[[deprecated("Remove in v4.1.  Use zeek::id::find() or zeek::id::find_val().")]]
extern zeek::TableVal* opt_internal_table(const char* name);	// nil if not defined

[[deprecated("Remove in v4.1.  Use zeek::id::find(), zeek::id::find_val(), and/or TableVal::ToPureListVal().")]]
extern zeek::ListVal* internal_list_val(const char* name);

[[deprecated("Remove in v4.1.  Use zeek::id::find_type().")]]
extern zeek::Type* internal_type(const char* name);

[[deprecated("Remove in v4.1.  Use zeek::id::find_func().")]]
extern zeek::Func* internal_func(const char* name);

[[deprecated("Remove in v4.1.  Use zeek::event_registry->Register().")]]
extern zeek::EventHandlerPtr internal_handler(const char* name);
