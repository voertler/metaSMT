#include "expressions.hpp"

#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/mpl/vector/vector50.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/iterator_range.hpp>

using namespace boost::python;

template<typename T>
logic_expression make_logic_expression( const T& v )
{
  return logic_expression( v );
}

logic_expression make_bit0()
{
  return logic_expression( metaSMT::logic::QF_BV::bit0 );
}

logic_expression make_bit1()
{
  return logic_expression( metaSMT::logic::QF_BV::bit1 );
}

logic_expression make_uint_expression( unsigned long value, unsigned long width )
{
  return bvuint_expression( value, width );
}

logic_expression make_sint_expression( long value, unsigned long width )
{
  return bvsint_expression( value, width );
}

template<typename T>
logic_expression make_str_expression( const std::string& value )
{
  return bvstr_expression<T>( value );
}

template<typename LogicTag, typename OpTag>
logic_expression make_unary_expression( const logic_expression& expr )
{
  return logic_expression( unary_expression<LogicTag, OpTag>( expr ) );
}

template<typename LogicTag, typename OpTag>
logic_expression make_binary_expression( const logic_expression& lhs, const logic_expression& rhs )
{
  return logic_expression( binary_expression<LogicTag, OpTag>( lhs, rhs ) );
}

template<typename LogicTag, typename OpTag>
logic_expression make_ternary_expression( const logic_expression& expr1, const logic_expression& expr2, const logic_expression& expr3 )
{
  return logic_expression( ternary_expression<LogicTag, OpTag>( expr1, expr2, expr3 ) );
}

template<typename T>
logic_expression make_extend_expression( unsigned long by, const logic_expression& expr )
{
  return logic_expression( extend_expression<T>( by, expr ) );
}

template<typename T>
logic_expression make_extract_expression( unsigned long from, unsigned long width, const logic_expression& expr )
{
  return logic_expression( extract_expression<T>( from, width, expr ) );
}

unsigned py_bitvector_width( const metaSMT::logic::QF_BV::bitvector& var )
{
  metaSMT::logic::QF_BV::tag::var_tag tag = boost::proto::value( var );
  return tag.width;
}

struct type_visitor : public boost::static_visitor<unsigned>
{
  unsigned operator()( bool expr ) const
  {
    return 0u;
  }

  unsigned operator()( const metaSMT::logic::QF_BV::QF_BV<boost::proto::terminal<metaSMT::logic::QF_BV::tag::bit0_tag>::type>& ) const
  {
    return 1u;
  }

  unsigned operator()( const metaSMT::logic::QF_BV::QF_BV<boost::proto::terminal<metaSMT::logic::QF_BV::tag::bit1_tag>::type>& ) const
  {
    return 1u;
  }

  unsigned operator()( const metaSMT::logic::predicate& pred ) const
  {
    return 0u;
  }

  unsigned operator()( const metaSMT::logic::QF_BV::bitvector& bv ) const
  {
    return 1u;
  }

  unsigned operator()( const bvuint_expression& expr ) const
  {
    return 1u;
  }

  unsigned operator()( const bvsint_expression& expr ) const
  {
    return 1u;
  }

  template<typename Tag>
  unsigned operator()( const bvstr_expression<Tag>& expr ) const
  {
    return 1u;
  }

  template<typename Tag>
  unsigned operator()( const unary_expression<logic_tag, Tag>& expr ) const
  {
    return 0u;
  }

  template<typename Tag>
  unsigned operator()( const unary_expression<bv_tag, Tag>& expr ) const
  {
    return 1u;
  }

  template<typename Tag>
  unsigned operator()( const binary_expression<logic_tag, Tag>& expr ) const
  {
    return 0u;
  }

  template<typename Tag>
  unsigned operator()( const binary_expression<bv_tag, Tag>& expr ) const
  {
    return 1u;
  }

  template<typename Tag>
  unsigned operator()( const ternary_expression<logic_tag, Tag>& expr ) const
  {
    return 0u;
  }

  template<typename Tag>
  unsigned operator()( const ternary_expression<bv_tag, Tag>& expr ) const
  {
    return 1u;
  }

  template<typename Tag>
  unsigned operator()( const extend_expression<Tag>& expr ) const
  {
    return 1u;
  }

  template<typename Tag>
  unsigned operator()( const extract_expression<Tag>& expr ) const
  {
    return 1u;
  }
};

unsigned py_logic_expression_type( const logic_expression& expr )
{
  return boost::apply_visitor( type_visitor(), expr );
}

void export_expressions()
{
  class_<logic_expression>( "logic_expression" )
    .def( "type", &py_logic_expression_type )
    ;
  def( "py_logic_term", &make_logic_expression<bool> );
  def( "py_bv_bit0", &make_bit0 );
  def( "py_bv_bit1", &make_bit1 );
  def( "py_logic_predicate", &make_logic_expression<metaSMT::logic::predicate> );
  def( "py_logic_bv", &make_logic_expression<metaSMT::logic::QF_BV::bitvector> );

  def( "py_bv_uint", &make_uint_expression );
  def( "py_bv_sint", &make_sint_expression );
  def( "py_bv_bin" , &make_str_expression<metaSMT::logic::QF_BV::tag::bvbin_tag> );
  def( "py_bv_hex" , &make_str_expression<metaSMT::logic::QF_BV::tag::bvhex_tag> );

  def( "py_logic_not"    , &make_unary_expression<logic_tag, tag::not_tag> );
  def( "py_bv_not"       , &make_unary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvnot_tag> );
  def( "py_bv_neg"       , &make_unary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvneg_tag> );

  def( "py_logic_equal"  , &make_binary_expression<logic_tag, tag::equal_tag> );
  def( "py_logic_nequal" , &make_binary_expression<logic_tag, tag::nequal_tag> );
  def( "py_logic_implies", &make_binary_expression<logic_tag, tag::implies_tag> );
  def( "py_logic_and"    , &make_binary_expression<logic_tag, tag::and_tag> );
  def( "py_logic_nand"   , &make_binary_expression<logic_tag, tag::nand_tag> );
  def( "py_logic_or"     , &make_binary_expression<logic_tag, tag::or_tag> );
  def( "py_logic_nor"    , &make_binary_expression<logic_tag, tag::nor_tag> );
  def( "py_logic_xor"    , &make_binary_expression<logic_tag, tag::xor_tag> );
  def( "py_logic_xnor"   , &make_binary_expression<logic_tag, tag::xnor_tag> );

  def( "py_bv_and"     , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvand_tag> );
  def( "py_bv_nand"    , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvnand_tag> );
  def( "py_bv_or"      , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvor_tag> );
  def( "py_bv_nor"     , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvnor_tag> );
  def( "py_bv_xor"     , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvxor_tag> );
  def( "py_bv_xnor"    , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvxnor_tag> );

  def( "py_bv_add"     , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvadd_tag> );
  def( "py_bv_mul"     , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvmul_tag> );
  def( "py_bv_sub"     , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvsub_tag> );
  def( "py_bv_sdiv"    , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvsdiv_tag> );
  def( "py_bv_srem"    , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvsrem_tag> );
  def( "py_bv_udiv"    , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvudiv_tag> );
  def( "py_bv_urem"    , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvurem_tag> );

  def( "py_bv_shl"     , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvshl_tag> );
  def( "py_bv_shr"     , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvshr_tag> );
  def( "py_bv_ashr"    , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvashr_tag> );

  def( "py_bv_comp"    , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::bvcomp_tag> );
  def( "py_bv_slt"     , &make_binary_expression<logic_tag, metaSMT::logic::QF_BV::tag::bvslt_tag> );
  def( "py_bv_sgt"     , &make_binary_expression<logic_tag, metaSMT::logic::QF_BV::tag::bvsgt_tag> );
  def( "py_bv_sle"     , &make_binary_expression<logic_tag, metaSMT::logic::QF_BV::tag::bvsle_tag> );
  def( "py_bv_sge"     , &make_binary_expression<logic_tag, metaSMT::logic::QF_BV::tag::bvsge_tag> );
  def( "py_bv_ult"     , &make_binary_expression<logic_tag, metaSMT::logic::QF_BV::tag::bvult_tag> );
  def( "py_bv_ugt"     , &make_binary_expression<logic_tag, metaSMT::logic::QF_BV::tag::bvugt_tag> );
  def( "py_bv_ule"     , &make_binary_expression<logic_tag, metaSMT::logic::QF_BV::tag::bvule_tag> );
  def( "py_bv_uge"     , &make_binary_expression<logic_tag, metaSMT::logic::QF_BV::tag::bvuge_tag> );

  def( "py_concat"     , &make_binary_expression<bv_tag, metaSMT::logic::QF_BV::tag::concat_tag> );
  def( "py_extract"    , &make_extract_expression<metaSMT::logic::QF_BV::tag::extract_tag> );
  def( "py_zero_extend", &make_extend_expression<metaSMT::logic::QF_BV::tag::zero_extend_tag> );
  def( "py_sign_extend", &make_extend_expression<metaSMT::logic::QF_BV::tag::sign_extend_tag> );

  def( "py_logic_ite"    , &make_ternary_expression<logic_tag, tag::ite_tag> );

  class_<metaSMT::logic::predicate>( "predicate" )
    ;
  def( "new_variable", &metaSMT::logic::new_variable );

  class_<metaSMT::logic::QF_BV::bitvector>( "bitvector" )
    .def( "width", &py_bitvector_width )
    ;
  def( "new_bitvector", &metaSMT::logic::QF_BV::new_bitvector );
}