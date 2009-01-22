/*
	remotehashmap.h - remote stdext::hash_map<> accessor helper class

	user mode GDI and USER handle leak tracker project
	Copyright (c) 2008 Dmitri Shelenin (deemok@gmail.com) 

	This program is free software; you can redistribute it and/or modify     
	it under the terms of the GNU General Public License as published by     
	the Free Software Foundation; either version 2 of the License, or        
	(at your option) any later version.                                      

	This program is distributed in the hope that it will be useful,          
	but WITHOUT ANY WARRANTY; without even the implied warranty of           
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            
	GNU General Public License for more details.                             

	You should have received a copy of the GNU General Public License        
	along with this program; if not, write to the Free Software              
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#pragma once

#include <engextcpp.hpp>

/*!
	\brief	Contains simplified implementation to access remote stdext::hash_map<> objects
*/
namespace dbgext
{
	extern size_t const _HASH_SEED;

	template<typename _Keytype>
	class _HashmapTraits
	{
	public:
		static _Keytype Map(ExtRemoteTyped data) {}
	};
	template<> ULONG _HashmapTraits<ULONG>::Map(ExtRemoteTyped data) {return data.GetUlong();}
	template<> HANDLE _HashmapTraits<HANDLE>::Map(ExtRemoteTyped data) {return reinterpret_cast<HANDLE>(data.GetUlong());}

	template<typename _Keytype, typename _Typetraits=_HashmapTraits<_Keytype>>
	class ExtRemoteHashMap
	{
		friend class iterator;
	public: // lifetime
		virtual ~ExtRemoteHashMap() {}
	public: // interface
		class iterator
		{
		public: // lifetime
			/*
				node {
					_Node *_Next;
					_Node *_Prev;
					value_type _Myval;	// std::pair<key,typename hashmap<>::value_type>
				}
			*/
			explicit iterator(ExtRemoteTyped node)
				: _Mynode(node)
			{

			}
			iterator(iterator const &_Right)
				: _Mynode(_Right._Mynode)
			{

			}
		public: // interface
			_Keytype key() {return _Typetraits::Map(_Mynode.Field("_Myval").Field("first"));}
			ExtRemoteTyped operator *() {return _Mynode.Field("_Myval").Field("second");}

			// iterations
			bool operator==(iterator const &__Right) {
				iterator &_Right=const_cast<iterator &>(__Right);
				return (_Mynode.GetPtr() == _Right._Mynode.GetPtr());
				}
			bool operator!=(iterator const &_Right) {return (!(*this == _Right));}
			//iterator operator+(size_t _Off) {iterator _Tmp=(*this); return (_Tmp += _Off);}
			iterator &operator++() {_Inc(); return (*this);}
			/*iterator operator++(int) {iterator _Tmp=(*this); _Inc(); return _Tmp;}
			iterator &operator+=(size_t _Off) {_Inc(_Off); return (*this);}
			iterator operator-(size_t _Off) {iterator _Tmp=(*this); return (_Tmp -= _Off);}
			iterator &operator--() {_Dec(); return (*this);}
			iterator operator--(int) {iterator _Tmp=(*this); _Dec(); return _Tmp;}
			//iterator &operator-=(size_t _Off) {return (*this += -_Off);}
			iterator &operator-=(size_t _Off) {_Dec(_Off); return (*this);}*/
		private: // implementation
			void _Inc(size_t _Steps=1) {while(_Steps--) _Mynode=_Mynode.Field("_Next");}
			/*void _Dec(size_t _Steps=1)
			{
				while(_Steps--) _Mynode=_Mynode.Field("_Prev");
			}*/
		private: // attrs
			ExtRemoteTyped _Mynode;
		};
	private: // implementation
		static size_t _Seed(_Keytype _Keyval) {return (size_t)_Keyval^_HASH_SEED;}
		static bool _Compare(_Keytype _Keyval1, _Keytype _Keyval2) {return (_Keyval1<_Keyval2);}

		class remote_vector
		{
		public: // lifetime
			explicit remote_vector(ExtRemoteTyped vector)
				: _Myvec(vector)
			{
			}
			~remote_vector() {}
		private: // implementation
			class iterator
			{
			public: // lifetime
				explicit iterator(ExtRemoteTyped ptr)
					: _Myptr(ptr)
					, _Ptr(_Myptr.Field("_Ptr"))
				{

				}
				iterator(iterator const &_Right)
					: _Myptr(_Right._Myptr)
					, _Ptr(_Right._Ptr)
				{

				}
			public: // interface
				typename ExtRemoteHashMap::iterator operator*() {return ExtRemoteHashMap::iterator(_Ptr);}
				iterator operator+(size_t _Off) {iterator _Tmp=(*this); return (_Tmp += _Off);}
				iterator &operator+=(size_t _Off) {_Inc(_Off); return (*this);}
			private: // implementation
				void _Inc(size_t _Off) 
				{
					DEBUG_TYPED_DATA &T=_Ptr.m_Typed;
					_Ptr.Set(false,T.ModBase,T.TypeId,T.Offset+_Off*_Myptr.GetTypeSize());
				}
			private: // attrs
				ExtRemoteTyped _Myptr, _Ptr;
			};
		public: // interface
			typename ExtRemoteHashMap::iterator operator[](size_t _Pos) {return *(begin()+_Pos);}
			typename remote_vector::iterator begin() {return iterator(_Myvec.Field("_Myfirst"));}
			typename remote_vector::iterator end() {return iterator(_Myvec.Field("_Mylast"));}
		private: // attrs
			ExtRemoteTyped _Myvec;
		};
		
	public: // interface
		/*!
			\return	position the iterator to the data identified via _Keyval
		*/
		iterator find(_Keytype _Keyval)	// @return std::list<>::iterator
		{	// find leftmost not less than _Keyval in mutable hash table
			size_t _Bucket=_Hashval(_Keyval);
			remote_vector vec(_Mymap.Field("_Vec"));
			iterator _End=vec[_Bucket+1];
			for(iterator _Where = vec[_Bucket]; _Where != _End; ++_Where)
			{
				if(!_Compare(_Where.key(),_Keyval))
				{
					return (_Compare(_Keyval,_Where.key()) ? (end()) : _Where);
				}
			}
			return (end());
		}
		iterator begin() {return iterator(_Mymap.Field("_List").Field("_Myhead").Field("_Next"));}
		iterator end() {return iterator(_Mymap.Field("_List").Field("_Myhead"));}
		size_t count() {return _Mymap.Field("_List").Field("_Mysize").GetUlong();}
	protected: // lifetime
		ExtRemoteHashMap(char const *expr)
			: _Mymap(expr)
		{

		}
	private: // implementation
		static _Keytype _Kfn(ExtRemoteTyped _Val) {return _Typetraits::Map(_Val.Field("first"));}
		size_t _Hashval(_Keytype _Keyval)
		{	// return hash value, masked and wrapped to current table size
			size_t _Mask = _Mymap.Field("_Mask").GetUlong();
			size_t _Num = _Seed(_Keyval) & _Mask;
			if (_Mymap.Field("_Maxidx").GetUlong() <= _Num)
				_Num -= (_Mask >> 1) + 1;
			return (_Num);
		}
	protected: // attrs
		ExtRemoteTyped _Mymap;
	};
} // dbgext namespace
