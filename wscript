APPNAME='suprem4'

top='.'
out='build/default'

import sys
import platform
import os

import waflib.Configure

waflib.Configure.autoconfig = True

def config_guess():
  guess={}
  guess['machine']  = platform.machine()
  guess['platform'] = platform.system()   #FIXME for jython, has to dig further about the underlying OS
  return guess

def options(opt):
  opt.load('compiler_c')
  opt.load('compiler_fc')
  
  guess = config_guess()
  platform = guess['platform']
  if guess['platform']=='Windows':
    opt.add_option('--target', action='store', default=None, dest='target_arch', help='target system architecture (ia32 intel64) [default: native]')
    opt.add_option('--crt', action='store', default='MT', dest='crt_version', help='Win32 only: CRT library version (MT MD) [default: MT]')
  
  
def configure(conf):
  guess = config_guess()

  platform = guess['platform']
  if platform=='Windows':
    target = conf.options.target_arch 
    if target==None:
      if   guess['machine']=='AMD64': target='intel64'
      elif guess['machine']=='x86':   target='ia32'
    conf.env.TARGET_ARCH = target
    conf.env['MSVC_VERSIONS'] = ['intel 12', 'intel 11']
    conf.env['MSVC_TARGETS']  = [target]

  if not platform=='Windows':
    conf.options.check_c_compiler = 'icc ' + conf.options.check_c_compiler 
    conf.options.check_fc = 'ifort ' + conf.options.check_fc

  conf.load('compiler_c compiler_fc')

  conf.env.PLATFORM = platform # Windows Linux Darwin ...

  # general C flags
  ccflags_common=[]
  ccflags_optimize=[]
  fcflags_common=[]
  fcflags_optimize=[]
  ldflags_common=[]
  ldflags_shlib=[]
  if platform=='Linux':
    if conf.env['COMPILER_CC'] in ['gcc', 'icc']:
      ccflags_common.extend(['-fPIC'])
      fcflags_common.extend(['-fPIC'])
      ccflags_optimize.extend(['-O2'])
      fcflags_optimize.extend(['-O2'])
      ldflags_shlib.extend(['-Wl,--export-dynamic'])

    ldflags_common.extend(['-lm'])
    if conf.env['COMPILER_CC'] in ['icc']:
      ldflags_common.extend(['-static-intel'])
  elif platform=='Windows':
    if conf.env['COMPILER_CC'] in ['msvc', 'icc']:
      ccflags_common.extend(['/EHsc'])
      ccflags_optimize.extend(['/O2'])
      if conf.options.crt_version=='MT':
        ccflags_common.extend(conf.env.CFLAGS_CRT_MULTITHREADED)
      else:
        ccflags_common.extend(conf.env.CFLAGS_CRT_MULTITHREADED_DLL)
        ldflags_common.append('/NODEFAULTLIB:LIBCMT')
    
  conf.env.append_value('CFLAGS', ccflags_common)
  conf.env.append_value('FCFLAGS', fcflags_common)
  
  conf.env.append_value('CFLAGS_opt', ccflags_optimize)
  conf.env.append_value('FCFLAGS_opt', fcflags_optimize)

  conf.env.append_value('LINKFLAGS', ldflags_common)
  conf.env.append_value('LINKFLAGS_cshlib', ldflags_shlib)
  
  if platform=='Windows':
    conf.check_libs_msvc('RPCRT4 User32')
  else:
    conf.define('PACKAGE_VERSION', '0.0')
 
  conf.env.append_value('DEFINES', ['DEVICE', 'SYSV', 'NO_F77'])

  def _split_win_paths(p):
    return p.split(';')
  if platform=='Windows':
    for i in ['LIBPATH']:
      plist = conf.env[i]
      newlist = []
      for p in plist: newlist.extend(_split_win_paths(p))
      conf.env[i] = newlist
   
  conf.write_config_header('config.h')

def build(bld):
  #print bld.env
  bld.recurse('src')

  #bld.shlib(
  #            source    = bld.path.ant_glob('nglib/nglib.cpp'),
  #            includes  = '. nglib libsrc/include',
  #            features  = 'cxx cxxshlib',
  #            target    = 'nglib',
  #            use       = 'ng_objects opt',
  #            install_path = '${PREFIX}/lib',
  #         )

  #bld.install_files('${PREFIX}/include', ['nglib/nglib.h'])

