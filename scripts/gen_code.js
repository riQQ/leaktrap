/*
	gen_code.js - interception code generator

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

// Add arg overrides similar to QString's
String.prototype.arg=function(arg) {
	return this.replace(/%\d+/i, arg);
};

coe=function(arg){WScript.StdOut.Write(arg);};
co=function(arg){WScript.StdOut.WriteLine(arg);};

if(WScript.Arguments.Named.Item("odir") === undefined)
{
	throw Error("Output directory not defined.\nUse gen_code[.js] /odir:<output_dir>");
}

(function(args){
	var work_dir=args;
	function array_map(arr1, arr2, map)
	{
		if(arr1.length!=arr2.length)
			throw Error("arrays are incompatible (size!=size)");
	
		var res=[];
		for(var k=0;k<arr1.length;k++)
		{
			res[k]=map(arr1[k],arr2[k]);
		}
		return res;
	}
	function String_Buffer()
	{
		var that=this;
		var quelle;
		var fso=new ActiveXObject("Scripting.FileSystemObject");
		function ident0(a)
		{
			var buf="";
			for(var i=0;i<that.indent;i++)
				buf += '\t';
			return buf+a;
		}
		function coe(arg)
		{
			quelle.Write(arg);
		};
		function co(arg)
		{
			quelle.WriteLine(arg);
		};
		function file(new_file)
		{
			if(quelle !== undefined)
				quelle.Close();
			quelle=fso.CreateTextFile(work_dir+"/"+new_file);
		};
		var ident=function(i){this.indent=i; return this;}
		var identp=function(){this.indent+=1; return this;}
		var identm=function(){this.indent-=1; return this;}
		//var o=function(a){this.buf+=ident0(a)+'\n';};
		var o0=function(a){co(a); return this;};
		var o=function(a){co(ident0(a)); return this;};
		//var oe=function(a){this.buf+=ident0(a);};
		var oe=function(a){coe(ident0(a)); return this;};
		//var set=function(a){this.buf=ident0(a);};
		var set=function(a){co(ident0(a)); return this;};
		var tos=function(){return this.buf;};
		// interface
		this.o=o;
		this.o0=o0;
		this.oe=oe;
		this.set=set;
		this.tos=tos;
		this.buf="";
		this.indent=0;
		this.ident=ident;
		this.p=identp;
		this.m=identm;
		this.file=file;
		return this;
	};

	var buf=new String_Buffer();

	// data types
	var dt_charptr=1,
	 dt_logfont=2,
	 dt_devmode=3,
	 dt_cdlgtemplate=4;
	// function tags
	var function_allocation=1,
	 function_cleanup=2;
	// alternative handle tag value minimum
	var USER_HANDLE_MIN=1000;
	
	var funs=
	{
		fs:{}, 
		handles:{	
			// GDI32
			PEN:'OBJ_PEN',BRUSH:'OBJ_BRUSH',DC:'OBJ_DC',METADC:'OBJ_METADC',PAL:'OBJ_PAL',FONT:'OBJ_FONT',
			BITMAP:'OBJ_BITMAP',REGION:'OBJ_REGION',METAFILE:'OBJ_METAFILE',MEMDC:'OBJ_MEMDC',
			EXTPEN:'OBJ_EXTPEN',ENHMETADC:'OBJ_ENHMETADC',ENHMETAFILE:'OBJ_ENHMETAFILE',
			COLORSPACE:'OBJ_COLORSPACE',
			// USER32
			ACCEL:USER_HANDLE_MIN,CARET:USER_HANDLE_MIN+1,CURSOR:USER_HANDLE_MIN+2,CONV:USER_HANDLE_MIN+3,
			CONVLIST:USER_HANDLE_MIN+4,WNDHOOK:USER_HANDLE_MIN+5,ICON:USER_HANDLE_MIN+6,MENU:USER_HANDLE_MIN+7,
			WINDOW:USER_HANDLE_MIN+8,DWP:USER_HANDLE_MIN+9
		}
	};	// map of function objects
	// format_fun(fun, ansi)
	funs.forall=function(format_fun, flag_fun)
	{
		for(var f in this.fs)
		{
			var fun=this.fs[f];
			if(flag_fun !== undefined && flag_fun(fun) || flag_fun === undefined)
			{
				if(fun.b)
				{
					format_fun(fun,true);
					format_fun(fun,false);
				}
				else
					format_fun(fun);
			}
		}
	};
	// format_fun(handle,handle_index)
	funs.withhandles=function(format_fun)
	{
		for(var h in this.handles)
		{
			format_fun(h,this.handles[h]);
		}
	};
	funs.set_cleanup=function(cleanup_id)
	{
		this.cleanup_id=cleanup_id;
		this.cleanup_cb=undefined;
	};
	funs.set_cleanupcallback=function(cleanup_cb)
	{
		this.cleanup_id=undefined;
		this.cleanup_cb=cleanup_cb;
	};
	funs.set_optimizenestedcalls=function(flag)
	{
		this.optimize_nested_calls=flag;
	}
	funs.set_host=function(host_module)
	{
		this.hm=host_module;
	};
	funs.make_function=function(name, val_return, args, ftype)
	{
		// b: ansi/unicode variants
		// g: undocumented function
		// h: host module
		// 
		var fun={a:name,b:false,c:val_return,d:args,type:ftype,id:name,g:false};
		function format_suffix(ansi)
		{
			if(ansi !== undefined)
			{
				if(ansi) return 'A';
				else return 'W';
			}
			return '';
		}
		fun.format_ftag=function(ansi)
		{
			return "Function_%1".arg(this.sig(ansi));
		};
		fun.format_ctag_call=function(ansi)
		{
			if(this.type&function_allocation)
			{
				if(this.ee !== undefined)	// cleanup callback defined
				{
					var params=[];
					/*var args=*/this.format_args(ansi, params);
					return "%1(%2)".arg(this.ee+format_suffix(ansi)).arg(params.join(","));
				}
				else
					return "Function_%1".arg(this.e);
			}
			else
				return this.format_ftag();
		};
		fun.format_ctag_decl=function(ansi)
		{
			if(this.type&function_allocation && this.ee !== undefined)
				return "%1(%2)".arg(this.ee+format_suffix(ansi)).arg(this.format_args(ansi).join(","));
			else
				throw Error("internal error");
		};
		fun.ctag=function()
		{
			if(this.type&function_cleanup)
				return "Function_%1".arg(this.id);
			else
				return this.format_ctag();
		};
		fun.sig=function(ansi)
		{
			return  this.a+format_suffix(ansi);
		};
		fun.sig0=function(ansi)
		{
			return  'Old'+this.a+format_suffix(ansi);
		};
		fun.sig1=function(ansi)
		{
			return  'My'+this.a+format_suffix(ansi);
		};
		fun.ptrsig=function(ansi, ptrname)
		{
			return "%1 (WINAPI *%2)(%3)".arg(this.c.toUpperCase()).arg(ptrname!==undefined?ptrname:'').arg(this.format_args(ansi).join(","));
		};
		fun.hsig=function()
		{
			return  "%1 handle".arg(this.parent.handles[this.hi]);
		};
		fun.ptr=function(ansi)
		{
			return this.sig();
		};
		fun.format_args=function(ansi, params)
		{
			function yield_param_name(k)
			{
				return 'a'+k;
			};
			var args=[];
			for(var k=0;k<this.d.length;k++)
			{
				if(params !== undefined)
				{
					params[k]=yield_param_name(k);
				}
				var arg=this.d[k];
				if(typeof arg == "string")
					args.push(arg.substr(0,1)=='^'?arg.substr(1):arg.toUpperCase());
				else
				if(typeof arg == "number" && ansi !== undefined)
				{
					switch(arg)
					{
						case dt_charptr:
							args.push(ansi?"char const *":"wchar_t const *");
							break;
						case dt_logfont:
							args.push(ansi?"LOGFONTA const *":"LOGFONTW const *");
							break;
						case dt_devmode:
							args.push(ansi?"DEVMODEA const *":"DEVMODEW const *");
							break;
						case dt_cdlgtemplate:
							args.push(ansi?"LPCDLGTEMPLATEA":"LPCDLGTEMPLATEW");
							break;
					}
						
				}
			}
			return args;
		};	// format_args
		this.fs[name]=fun;
		fun.parent=this;
		return fun;
	}
	funs.add_function=function(name, val_return, args)
	{
		var that=this;
		var fun=this.fs[name];
		if(fun === undefined)
		{
			fun=this.make_function(name, val_return, args, function_allocation);
			fun.e=this.cleanup_id;	// currently active cleanup fun id
			fun.ee=this.cleanup_cb;	// currently active cleanup callback fun id
			fun.h=this.hm;
		}
		else
		{
			fun.type |= function_allocation;
		}
		
		fun.optimize_nested_calls=this.optimize_nested_calls;
		return fun;
	};
	funs.add_cleanupfunction=function(name, val_return, args)
	{
		var fun=this.fs[name];
		if(fun === undefined)
		{
			fun=this.make_function(name, val_return, args, function_cleanup);
		}
		else
		{
			fun.type |= function_cleanup;
		}
		fun.f=0;	// default handle index
		
		return fun;
	};
	funs.gen_code=function()
	{
		buf.file("Intercepts.cpp");
		// generate source file
		print_header();

		// generate function pointers
		buf.o("//").o("// function pointers").o("//");
		this.forall(print_funptr);
		buf.o("//").o("// cleanup callbacks").o("//");
		this.forall(print_cleanupcallback,function(fun){return fun.ee !== undefined;});
		buf.o("//").o("// api detours").o("//");
		this.forall(format_detour);
		print_implementation();
		print_footer();

		print_mappings();

		// generate header file
		buf.file("Intercepts.h");
		print_header_h();
		print_interface();
		print_footer_h();
	};
	print_copyleft=function(desc)
	{
		buf.o("/*");
		buf.p().
		o("%1\n".arg(desc)).

		o("user mode GDI and USER handle leak tracker project").
		o("Copyright (c) 2008 Dmitri Shelenin (deemok@gmail.com)\n").

		o("This program is free software; you can redistribute it and/or modify     ").
		o("it under the terms of the GNU General Public License as published by     ").
		o("the Free Software Foundation; either version 2 of the License, or        ").
		o("(at your option) any later version.\n").

		o("This program is distributed in the hope that it will be useful,          ").
		o("but WITHOUT ANY WARRANTY; without even the implied warranty of           ").
		o("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            ").
		o("GNU General Public License for more details.\n").

		o("You should have received a copy of the GNU General Public License        ").
		o("along with this program; if not, write to the Free Software              ").
		o("Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n").
		
		m().o("*/");
	}
	print_header=function()
	{
		print_copyleft("intercepts.cpp - allocation/cleanup interception");

		buf.o("/*").p().
		o("Generated on %1. Do not EDIT!!!".arg((new Date).toLocaleString())).
		m().o("*/");


		buf.o("#define _WIN32_WINNT 0x0501	// we need WinXP anyways");
		buf.o("#include <windows.h>");
		buf.o("#include <wingdi.h>");
		buf.o("#include <detours.h>");
		buf.o("#include \"leaktrap.h\"\t//import/export macros");
		buf.o("#include \"Intercepts.h\"");
		buf.o("#include \"impl.h\"");
		buf.o("").o("");
		buf.o("namespace detail\n{");
		print_header_extras();
		buf.p();
	};
	print_header_h=function()
	{
		print_copyleft("intercepts.h - allocation/cleanup interception");

		buf.o("/*").p().
		o("Generated on %1. Do not EDIT!!!".arg((new Date).toLocaleString())).
		m().o("*/");
		
		buf.o("#pragma once\n\n");
		buf.o("namespace detail\n{");
		buf.p();
	};
	print_footer=function()
	{
		buf.m();
		buf.o("} // namespace detail\n");
		buf.o("// Intercepts.cpp\n");
	};
	print_footer_h=function()
	{
		buf.m();
		buf.o("} // namespace detail\n");
		buf.o("// Intercepts.h\n");
	};
	function print_header_extras()
	{
		buf.p().o("namespace").o("{").p();
		buf.o("// stack capture helper");
		buf.o("USHORT (WINAPI *pfnCaptureStackBackTrace)(ULONG,ULONG,PVOID*,PULONG)=0;");
		buf.m().o("} // internals").o("").m();
	}
	function print_interface()
	{
		// typedefs
		print_functiontags();
		print_handletags();

		buf.o("bool DetourAll();");
		buf.o("void UndetourAll();");
		buf.o("char const *From(function_tag);");
		buf.o("char const *From(handle_tag);");
		buf.o("handle_tag GetHandleTag(function_tag ftag, void* handle);");
		buf.o("bool IsGdiHandle(handle_tag);");
	}
	function print_mappings()
	{
		buf.file("Mappings.cpp");
		
		print_copyleft("mappings.cpp - various mapping helpers");

		buf.o("/*").p().
		o("Generated on %1. Do not EDIT!!!".arg((new Date).toLocaleString())).
		m().o("*/");
		
		buf.o("#define _WIN32_WINNT 0x0501	// we need WinXP anyways");
		buf.o("#include <windows.h>");
		buf.o("#include <wingdi.h>");
		buf.o("#include \"Intercepts.h\"");
		buf.o("").o("");
		buf.o("namespace detail\n{");
		buf.p();
	
		// char const *From(function_tag)
		buf.o("char const *From(function_tag tag)").o("{").p();
			buf.o("switch(tag)").o("{").p();
				funs.forall(print_ftag_format);
				// default case
				buf.o("case Function_Undefined:").o("default:").o("{").p();
					//buf.o("qqDebug(\"undefined function tag in From(function_tag): 0x%lx\",ftag);");
					buf.o("return \"\";");
				buf.m().o("}");
			buf.m().o("}");
		buf.m().o("}");
		
		// char const *From(handle_tag)
		buf.o("char const *From(handle_tag tag)").o("{").p();
			buf.o("switch(tag)").o("{").p();
				funs.withhandles(print_htag_format);
				// default case
				buf.o("case Handle_Undefined:").o("default:").o("{").p();
					//buf.o("qqDebug(\"undefined function tag in From(function_tag): 0x%lx\",ftag);");
					buf.o("return \"\";");
				buf.m().o("}");
			buf.m().o("}");
		buf.m().o("}");

		// bool IsGdiHandle(handle_tag)
		buf.o("bool IsGdiHandle(handle_tag tag)").o("{").p();
			buf.o("switch(tag)").o("{").p();
				buf.o("	// fall throughs");
				funs.withhandles(print_gdi_htags)
				buf.o("{").p();
					buf.o("return true;");
				buf.m().o("}");
				buf.o("	// fall throughs");
				funs.withhandles(print_nongdi_htags);
				// +default case
				buf.o("case Handle_Undefined:").o("default:").o("{").p();
					buf.o("return false;");
				buf.m().o("}");
			buf.m().o("}");
		buf.m().o("}");

		buf.m().o("} // namespace detail\n").o("// Mappings.cpp\n");
	}
	function print_implementation()
	{
		// DetourAll()
		buf.o("bool DetourAll()").o("{").p();
			buf.o("// initialize stack capture routine");
			buf.o("pfnCaptureStackBackTrace = ").p();
			buf.o("(USHORT (WINAPI *)(ULONG, ULONG, PVOID*, PULONG))::GetProcAddress(").p();
				buf.o("::GetModuleHandleW(L\"ntdll.dll\"), ");
				buf.o("\"RtlCaptureStackBackTrace\"").m();
			buf.o(");").m();
			buf.o("if(!pfnCaptureStackBackTrace)").o("{").p();
				buf.o("qqDebug(\"RtlCaptureStackBackTrace not available!\");");
			buf.o("return false;").m().o("}");

			// lookup undocumented function(s)
			funs.forall(format_undocfun_init, function(fun){return fun.g;});

			buf.o("DetourTransactionBegin();");
			buf.o("DetourUpdateThread(::GetCurrentThread());");
			buf.o("BOOL bOk=TRUE;").o("LONG Status=0L;");
		
			funs.forall(format_detourapply);
			
			buf.o0("Epilog:");
			
			buf.o("if(bOk)").o("{").p().o("DetourTransactionCommit();").o("return true;").m().o("}");
			buf.o("else").o("{").p();
			buf.o("qqDebug(\"DetourAll() failed to detour one (or more) functions\");");
			buf.o("DetourTransactionAbort();").o("return false;").m().o("}");
		buf.m().o("}");
		
		// UndetourAll()
		buf.o("void UndetourAll()").o("{").p();
			buf.o("DetourTransactionBegin();");
			buf.o("DetourUpdateThread(::GetCurrentThread());");
			funs.forall(format_detourunapply);
			buf.o("DetourTransactionCommit();");
		buf.m().o("}");
	}
	function print_functiontags()
	{
		buf.o("typedef enum").o("{").p();
		buf.o("Function_Undefined=-1,");
		funs.forall(function(fun,ansi){buf.o("%1".arg(fun.format_ftag(ansi)+','));});
		
		buf.m().o("} function_tag;");
	}
	function print_handletags()
	{
		buf.o("typedef enum").o("{").p();
		buf.o("Handle_Undefined=-1,");
		funs.withhandles(function(h,id){buf.o("Handle_%1=%2,".arg(h).arg(id));});
		
		buf.m().o("} handle_tag;");
	}
	function print_ftag_format(fun, ansi)
	{
		buf.o("case %1:".arg(fun.format_ftag(ansi))).o("{").p();
			buf.o("return \"%1\";".arg(fun.sig(ansi)));
		buf.m().o("}");
	}
	function print_htag_format(h, id)
	{
		buf.o("case Handle_%1:".arg(h.toString())).o("{").p();
			buf.o("return \"%1\";".arg(h));
		buf.m().o("}");
	}
	function print_gdi_htags(h, id)
	{
		// @ds: for the time being, very extraordindaire and mind-boggling
		if(typeof id === 'string')
			buf.o("case Handle_%1:".arg(h.toString()));
	}
	function print_nongdi_htags(h, id)
	{
		// @ds: for the time being, very extraordindaire and mind-boggling
		if(id>=USER_HANDLE_MIN)
			buf.o("case Handle_%1:".arg(h.toString()));
	}
	function format_detourapply(fun, ansi)
	{
		buf.o("Status=DetourAttach(&(PVOID&)%1,%2);".arg(fun.sig0(ansi)).arg(fun.sig1(ansi)));
		buf.o("if(NO_ERROR!=Status)");
		buf.o("{"); buf.p();
			buf.o("qqDebug(\"failed to detour '%1'\");".arg(fun.a));
			buf.o("bOk=FALSE;").o("goto Epilog;");
		buf.m();buf.o("}"); 
	}
	function format_detourunapply(fun, ansi)
	{
		buf.o("DetourDetach(&(PVOID&)%1,%2);".arg(fun.sig0(ansi)).arg(fun.sig1(ansi)));
	}
	function format_undocfun_init(fun, ansi)
	{
		var ptr=fun.ptr();
		buf.o("// initialize %1".arg(fun.a));
		buf.o("%1 = ".arg(fun.ptrsig(ansi,ptr))).p();
		buf.o("(%1)::GetProcAddress(".arg(fun.ptrsig())).p();
			buf.o("::GetModuleHandleW(L\"%1\"), ".arg(fun.h));
			buf.o("\"%1\"".arg(fun.a)).m();
		buf.o(");").m();
		buf.o("if(!%1)".arg(ptr)).o("{").p();
			buf.o("qqDebug(\"'%1!%2' not available!\");".arg(fun.h).arg(fun.a));
		buf.o("return false;").m().o("}");
		buf.o("%1=%2;".arg(fun.sig0()).arg(ptr));
	}
	function format_detour(fun, ansi)
	{
		var params=[];
		var args=fun.format_args(ansi, params);
		var sig=array_map(args, params, function(a1,a2){return a1+" "+a2;}).join(",");
		var ret=fun.c.toUpperCase();
		//var gdi=("gdi32.dll" == fun.h);
		var optimize_nested_calls=fun.optimize_nested_calls !== undefined && fun.optimize_nested_calls;
		
		// signature
		buf.o("%1 WINAPI %2(%3)".arg(ret).arg(fun.sig1(ansi)).arg(sig)).o("{").p();
		
			//if(gdi && fun.type!=function_cleanup || fun.e === "")	// do this only for allocations or allocation/cleanup combos
			if(optimize_nested_calls)
			{
				// read nested-call indicator
				buf.o("bool flat_call=enter();");
			}
		
			// body
			buf.o("%1 ret_handle=%2(%3);".arg(fun.c.toUpperCase()).arg(fun.sig0(ansi)).arg(params.join(",")));
			
			// dump handle information in the debugger
			//buf.o0("#ifdef DEBUG").o("qqDebug(\"%1, handle=0x%lx\", ret_handle);".arg(fun.sig(ansi))).o0("#endif // DEBUG");

			if(fun.type & function_allocation && (fun.ee !== undefined || fun.e != ""))
			{
				if(fun.ee !== undefined)
					buf.o("cleanup_callback_ret s=%1;".arg(fun.format_ctag_call(ansi)));
				else
				{
					buf.o("cleanup_callback_ret s;").o("s.FunctionTag=TRUE;").o("s.ctag=%1;".arg(fun.format_ctag_call(ansi)));
				}
			}

			//if(gdi && fun.type!=function_cleanup || fun.e === "")	// do this only for allocations or allocation/cleanup combos
			if(optimize_nested_calls)
			{
				buf.o("if(!flat_call)").o("{").p().o("goto Epilog;").m().o("}");
			}

			/*if(fun.e === "")
			{
				buf.o("qqDebug(\"%1(): allocated shared resource, handle=%lx\", ret_handle);".arg(fun.a));
			} */

			if(fun.type & function_allocation && (fun.ee !== undefined || fun.e != ""))
			{
				buf.o("// perform allocation tasks");
				buf.o("if(0!=ret_handle && s.FunctionTag)").o("{").p();	// only sample stacks for valid handles and when cleanup_callback defines a function tag
				
				buf.o("DWORD stack[32];");
				buf.o("ULONG traceHash=0;");
				buf.o("SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);");
				buf.o("FILETIME ts={0,0};");
				buf.o("::GetSystemTimeAsFileTime(&ts);");
				buf.o("allocations[ret_handle]=allocation(traceHash,ts);");
				
				buf.o("if(!traceHash)	// has not been properly calculated; must be a shorty stack!").o("{").p();
					buf.o("qqDebug(\"**********************\");");
					buf.o("qqDebug(\"Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!\");")
					buf.o("qqDebug(\"**********************\");");
				buf.m().o("}").o("else").o("{").p();
				
				buf.o("stackdb_type::iterator it=stackdb.find(traceHash);");
				buf.o("if(it==stackdb.end())	// no matching stack record exists").o("{").p();
					var ftag=fun.format_ftag(ansi);
					if(fun.h === "gdi32.dll")
					{
						buf.o("DWORD obj_type=::GetObjectType(ret_handle);");
						buf.o("if(0==obj_type)	// unable to determine object type").o("{").p();
							buf.o("qqDebug(\"Unable to determine GDI object type for handle=%lx, error=%lx\",ret_handle,::GetLastError());");
							buf.o("if(::IsDebuggerPresent()) ::DebugBreak();");
						buf.m().o("}");
						buf.o("handle_tag htag=static_cast<handle_tag>(obj_type);");
					}
					else
					{
						buf.o("handle_tag htag=GetHandleTag(%1,ret_handle);".arg(ftag));
					}
					buf.o("stackdb[traceHash]=stack_trace(%1,s.ctag,htag,stack,frames);".arg(ftag));
				buf.m().o("}").o("else").o("{").p();

					buf.o("it->second.allocationCount_ += 1;	// update allocation counter");
				buf.m().o("}").m().o("}");
				buf.m().o("}");
			}
			
			if(fun.type & function_cleanup)
			{
				buf.o("// perform cleanup tasks").o("{").p();
				
				buf.o("%1 handle=a%2;".arg(args[fun.f]).arg(fun.f));
				
				buf.o("allocations_type::iterator it=allocations.find(handle);");
				buf.o("if(it != allocations.end())	// matched an allocation").o("{").p();
				
				// if <---
					buf.o("stackdb_type::iterator sit=stackdb.find(it->second.hash_);");
					buf.o("if(sit != stackdb.end())").o("{").p();
						buf.o("// look up corresponding stack trace record and remove it");
						buf.o("sit->second.allocationCount_ -= 1;	// update allocation counter");
						buf.o("// check allocation/cleanup consistency");
						buf.o("if(%1 != sit->second.ctag_)".arg(fun.ctag())).o("{").p();
							buf.o("qqDebug(\"**********************\");");
							buf.o("qqDebug(\"* Inconsistent cleanup: %1(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx\", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);".arg(fun.a));
							buf.o("qqDebug(\"**********************\");");
							buf.o("if(::IsDebuggerPresent()) ::DebugBreak();");
							buf.m().o("}");
							buf.o("allocations.erase(it); // dump this allocation");
						buf.m().o("}");

				// --->
				buf.o("else").o("{").p();
				
					buf.o("// no allocations recorded for this cleanup");
					buf.o("qqDebug(\"**********************\");");
					buf.o("qqDebug(\"* Inconsistent cleanup: no allocation record for handle 0x%lx!\",handle);");
					buf.o("qqDebug(\"**********************\");");
				
				// footer
				buf.m().o("}").m().o("}").m().o("}");
			}
			
			//if(gdi && fun.type!=function_cleanup || fun.e === "")	// do this only for allocations or allocation/cleanup combos
			if(optimize_nested_calls)
			{
				buf.o("leave();");
				buf.o0("Epilog:");
			}
			
			buf.o("return ret_handle;");

		// footer
		buf.m().o("}");
	}

	function print_funptr(fun, ansi)
	{
		var args=fun.format_args(ansi).join(",");
		buf.o("%1 = %2;".arg(fun.ptrsig(ansi,fun.sig0(ansi))).arg(fun.g?'0':fun.sig(ansi)));
	}

	function print_cleanupcallback(fun, ansi)
	{
		buf.o("extern cleanup_callback_ret %1;".arg(fun.format_ctag_decl(ansi)));
	}

	with(funs)
	{
		set_optimizenestedcalls(true);
		set_host("gdi32.dll");
		// resource allocation
		set_cleanup("DeleteObject");
		add_function("LoadBitmap", "hbitmap", ["hinstance",dt_charptr]).b=true;
		add_function("CreateBitmap", "hbitmap", ["int","int","UINT","UINT","void const *"]);
		add_function("CreateBitmapIndirect", "hbitmap", ["BITMAP const *"]);
		add_function("CreateCompatibleBitmap", "hbitmap", ["HDC","int","int"]);
		add_function("CreateDiscardableBitmap", "hbitmap", ["HDC","int","int"]);
		add_function("CreateDIBitmap", "hbitmap", ["HDC","BITMAPINFOHEADER const *","DWORD","void const *","BITMAPINFO const *","UINT"]);
		add_function("CreateDIBSection", "hbitmap", ["HDC","BITMAPINFO const *","UINT","void **","HANDLE","DWORD"]);	
		add_function("CreateBrushIndirect","hbrush",["LOGBRUSH const *"]);
		add_function("CreateSolidBrush","hbrush",["COLORREF"]);
		add_function("CreatePatternBrush","hbrush",["HBITMAP"]);
		add_function("CreateDIBPatternBrush","hbrush",["HGLOBAL","UINT"]);
		add_function("CreateDIBPatternBrushPt","hbrush",["void const *","UINT"]);
		add_function("CreateHatchBrush","hbrush",["int","COLORREF"]);
		add_function("CreateFont","hfont",["int","int","int","int","int","DWORD","DWORD","DWORD","DWORD","DWORD","DWORD","DWORD","DWORD",dt_charptr]).b=true;
		add_function("CreateFontIndirect","hfont",[dt_logfont]).b=true;
		add_function("CreatePen","hpen",["int","int","COLORREF"]);
		add_function("CreatePenIndirect","hpen",["LOGPEN const *"]);
		add_function("ExtCreatePen","hpen",["DWORD","DWORD","LOGBRUSH const *","DWORD","DWORD const *"]);
		add_function("PathToRegion","hrgn",["HDC"]);
		add_function("CreateEllipticRgn","hrgn",["int","int","int","int"]);
		add_function("CreateEllipticRgnIndirect","hrgn",["RECT const *"]);
		add_function("CreatePolygonRgn","hrgn",["POINT const *","int","int"]);
		add_function("CreatePolyPolygonRgn","hrgn",["POINT const *","INT const *","int","int"]);
		add_function("CreateRectRgn","hrgn",["int","int","int","int"]);
		add_function("CreateRectRgnIndirect","hrgn",["RECT const *"]);
		add_function("CreateRoundRectRgn","hrgn",["int","int","int","int","int","int"]);
		add_function("ExtCreateRegion","hrgn",["XFORM const *","DWORD","RGNDATA const *"]);
		add_function("CreateHalftonePalette","hpalette",["HDC"]);
		add_function("CreatePalette","hpalette",["LOGPALETTE const *"]);
		
		set_cleanup("ReleaseDC");
		add_function("GetWindowDC", "hdc", ["hwnd"]);
		
		set_cleanup("DeleteDC");
		add_function("CreateCompatibleDC","hdc",["HDC"]);
		add_function("CreateDC","hdc",[dt_charptr,dt_charptr,dt_charptr,dt_devmode]).b=true;
		add_function("CreateIC","hdc",[ dt_charptr,dt_charptr,dt_charptr,dt_devmode]).b=true;
		
		set_cleanup("CloseMetaFile");
		add_function("CreateMetaFile","hdc",[dt_charptr]).b=true;
		
		set_cleanup("CloseEnhMetaFile");
		add_function("CreateEnhMetaFile","hdc",["HDC",dt_charptr,"RECT const *",dt_charptr]).b=true;
		
		set_cleanup("DeleteEnhMetaFile");
		add_function("GetEnhMetaFile","henhmetafile",[dt_charptr]).b=true;
		add_function("CloseEnhMetaFile","henhmetafile",["HDC"]);
		
		set_cleanup("DeleteMetaFile");
		add_function("CloseMetaFile","hmetafile",["HDC"]);
		
		// experimental USER handle support
		set_host("user32.dll");
		set_cleanup("DestroyIcon");
		add_function("CreateIconFromResourceEx","hicon",["pbyte","dword","bool","dword","int","int","uint"]);
		add_function("CreateIconIndirect","hicon",["PICONINFO"]);
		add_function("CopyIcon","hicon",["hicon"]);
		add_function("ExtractIcon","hicon",["hinstance",dt_charptr,"uint"]).b=true;
		//LoadIconA falls back to LoadIconW() which falls back to LoadIcoCur!
		//add_function("LoadIcoCur","hicon",["hinstance","wchar_t const *","int","int","int","uint"]);
		
		// multiple cleanup options
		set_cleanupcallback("CopyImageCleanupCallback");
		add_function("CopyImage","handle",["handle","uint","int","int","uint"]);
		set_cleanupcallback("LoadImageCleanupCallback");	
		add_function("LoadImage","handle",["hinstance",dt_charptr,"uint","int","int","uint"]).b=true;

		set_cleanupcallback("GetDCExCleanupCallback");
		add_function("GetDCEx", "hdc", ["hwnd","hrgn","dword"]);
		set_cleanupcallback("GetDCCleanupCallback");
		add_function("GetDC", "hdc", ["hwnd"]);

		// empty cleanup; these APIs do not require cleanup!
		set_cleanup("");
		add_function("DrawState","bool",["hdc","hbrush","DRAWSTATEPROC","lparam","wparam","int","int","int","int","uint"]).b=true;
		add_function("LoadCursor","hcursor",["hinstance",dt_charptr]).b=true;
		add_function("LoadCursorFromFile","hcursor",[dt_charptr]).b=true;
		// @ds: LoadIconA/W is used to create a shared resource
		add_function("LoadIcon","hicon",["hinstance",dt_charptr]).b=true;

		set_cleanup("DestroyAcceleratorTable");
		add_function("CreateAcceleratorTable","haccel",["LPACCEL","int"]).b=true;
		
		set_cleanup("DestroyCursor");
		add_function("CreateCursor","hcursor",["hinstance","int","int","int","int","const VOID *","const VOID *"]);
		
		set_cleanup("DdeDisconnect");
		add_function("DdeConnect","hconv",["dword","hsz","hsz","PCONVCONTEXT"]);
		
		set_cleanup("DdeDisconnectList");
		add_function("DdeConnectList","hconvlist",["dword","hsz","hsz","HCONVLIST","PCONVCONTEXT"]);
		
		//set_cleanup("UnhookWindowsHookEx");
		set_cleanupcallback("SetWindowsHookCallback");
		add_function("SetWindowsHookEx","hhook",["int","hookproc","hinstance","dword"]);
		
		set_cleanup("DestroyMenu");
		add_function("CreateMenu","hmenu",[]);
		add_function("CreatePopupMenu","hmenu",[]);
		add_function("LoadMenu","hmenu",["hinstance",dt_charptr]).b=true;
		add_function("LoadMenuIndirect","hmenu",["MENUTEMPLATE CONST *"]);
		
		set_cleanup("EndDeferWindowPos");
		add_function("BeginDeferWindowPos","hdwp",["int"]);
		
		set_optimizenestedcalls(false);	// do not optimize for cleanups
		
		//set_cleanup("DestroyWindow");
		set_cleanupcallback("CreateWindowCallback");
		//add_function("CreateWindow","hwnd",[dt_charptr,dt_charptr,"dword","int","int","int","int","hwnd","hmenu","hinstance","lpvoid"]).b=true;
		add_function("CreateWindowEx","hwnd",["dword",dt_charptr,dt_charptr,"dword","int","int","int","int","hwnd","hmenu","hinstance","lpvoid"]).b=true;
		add_function("CreateDialogParam","hwnd",["hinstance",dt_charptr,"hwnd","dlgproc","lparam"]).b=true;
		add_function("CreateDialogIndirectParam","hwnd",["hinstance",dt_cdlgtemplate,"hwnd","dlgproc","lparam"]).b=true;
		add_function("CreateMDIWindow","hwnd",[dt_charptr,dt_charptr,"dword","int","int","int","int","hwnd","hinstance","lparam"]).b=true;
		
		// wannabes
		//set_cleanup("DestroyCaret");
		//add_function("CreateCaret","hcaret",['LPACCEL','int']).b=true;

		// resource cleanup
		add_cleanupfunction("DeleteObject","bool",["HGDIOBJ"]);
		add_cleanupfunction("DeleteDC","bool",["HDC"]);
		add_cleanupfunction("DeleteMetaFile","bool",["HMETAFILE"]);
		add_cleanupfunction("DeleteEnhMetaFile","bool",["HENHMETAFILE"]);
		// moved to resource allocation section
		add_cleanupfunction("CloseMetaFile","hmetafile",["HDC"]);
		add_cleanupfunction("CloseEnhMetaFile","henhmetafile",["HDC"]);
		add_cleanupfunction("ReleaseDC","int",["HWND","HDC"]).f=1;
		add_cleanupfunction("DestroyIcon","bool",["hicon"]);
		add_cleanupfunction("DestroyCursor","bool",["hcursor"]);
		add_cleanupfunction("DdeDisconnect","bool",["hconv"]);
		add_cleanupfunction("DdeDisconnectList","bool",["hconvlist"]);
		add_cleanupfunction("DestroyWindow","bool",["hwnd"]);
		add_cleanupfunction("EndDeferWindowPos","bool",["hdwp"]);
		add_cleanupfunction("DestroyAcceleratorTable","bool",["haccel"]);
		add_cleanupfunction("UnhookWindowsHookEx","bool",["hhook"]);
		add_cleanupfunction("DestroyMenu","bool",["hmenu"]);

		// generate code
		gen_code();
	};	// with(funs)
})(WScript.Arguments.Named.Item("odir"));

