#!/usr/bin/python

###########################################
## Common section, for loading the tools

## Load the builders in config
env = Environment(TARGS=COMMAND_LINE_TARGETS, ARGS=ARGUMENTS, tools=['default', 'generic', 'kde', 'codeine'], toolpath=['./scons/'])


## the configuration should be done by now, quit
if 'configure' in COMMAND_LINE_TARGETS:
   env.Exit(0)



"""
Overview of the module system :

Each module (kde.py, generic.py, sound.py..) tries to load a stored
configuration when run. If the stored configuration does not exist
or if 'configure' is given on the command line (scons configure),
the module launches the verifications and detectioins and stores
the results. Modules also call exit when the detection fail.

For example, kde.py stores its config into kde.cache.py

This has several advantages for both developers and users :
  - Users do not have to run ./configure to compile
  - The build is insensitive to environment changes
  - The cache maintains the objects so the config can be changed often
  - Each module adds its own help via env.Help("message")
"""

## Use the variables available in the environment - unsafe, but moc, meinproc need it :-/
import os
env.AppendUnique( ENV = os.environ )
## If you do not want to copy the whole environment, you can use this instead (HOME is necessary for uic):
#env.AppendUnique( ENV = {'PATH' : os.environ['PATH'], 'HOME' : os.environ['HOME']} )

## The target make dist requires the python module shutil which is in 2.3
env.EnsurePythonVersion(2, 3)

## Bksys requires scons 0.96
env.EnsureSConsVersion(0, 96)

"""
Explanation of the 'env = Environment...' line :
* the command line arguments and targets are stored in env['TARGS'] and env['ARGS'] for use by the tools
* the part 'tools=['default', 'generic ..' detect and load the necessary functions for doing the things
* the part "toolpath=['./']" tells that the tools can be found in the current directory (generic.py, kde.py ..)
"""

"""
To load more configuration modules one should only have to add the appropriate tool
ie: to detect alsa and add the proper cflags, ldflags ..
    a file alsa.py file will be needed, and one should then use :
    env = Environment(TARGS=COMMAND_LINE_TARGETS, ARGS=ARGUMENTS, tools=['default', 'generic', 'kde', 'alsa'], toolpath=['./'])

You can also load environments that are targetted to different platforms
ie: if os.sys.platform = "darwin":
	env = Environment(...
    elsif os.sys.platform = "linux":
	env = Environment(...

"""

## Setup the cache directory - this avoids recompiling the same files over and over again
## this is very handy when working with cvs
env.CacheDir('cache')
env.SConsignFile('scons/signatures')

## If you need more libs and they rely on pkg-config
## ie: add support for GTK (source: the scons wiki on www.scons.org)
# env.ParseConfig('pkg-config --cflags --libs gtk+-2.0')

"""
This tell scons that there are no rcs or sccs files - this trick
can speed up things a bit when having lots of #include
in the source code and for network file systems
"""
env.SourceCode(".", None)
dirs = [ '.', 'src', 'src/part', 'src/app' ]
for dir in dirs:
	env.SourceCode(dir, None)

## If we had only one program (named kvigor) to build,
## we could add before exporting the env (some kde
## helpers in kde.py need it) :
# env['APPNAME'] = 'kvigor'

## Use this define if you are using the kde translation scheme (.po files)
env.Append( CPPFLAGS = ['-DQT_NO_TRANSLATION'] )

## Uncomment the following if you need threading support threading
#env.Append( CPPFLAGS = ['-DQT_THREAD_SUPPORT', '-D_REENTRANT'] )
#if os.uname()[0] == "FreeBSD":
#	env.Append(LINKFLAGS=["-pthread"])

## Important : export the environment so that SConscript files can the
## configuration and builders in it
Export("env")


def string_it(target, source, env):
    print "Visit #codeine on irc.freenode.net!"
    return 0

env.AddPostAction( "install", string_it )

env.SConscript( "src/SConscript", build_dir='build', duplicate=0 )


if 'dist' in COMMAND_LINE_TARGETS:

      APPNAME = 'codeine'
      VERSION = os.popen("cat VERSION").read().rstrip()
      FOLDER  = APPNAME+'-'+VERSION
      ARCHIVE = FOLDER+'.tar.bz2'

      GREEN  ="\033[92m"
      NORMAL ="\033[0m"

      import shutil
      import glob

      ## check if the temporary directory already exists
      if os.path.isdir(FOLDER):
            shutil.rmtree(FOLDER)

      ## create a temporary directory
      startdir = os.getcwd()
      # TODO copying the cache takes forever! delete it first
      shutil.copytree(startdir, FOLDER)

      ## remove the unnecessary files
      os.popen("find "+FOLDER+" -name \"{arch}\" | xargs rm -rf")
      os.popen("find "+FOLDER+" -name \".arch-ids\" | xargs rm -rf")
      os.popen("find "+FOLDER+" -name \".arch-inventory\" | xargs rm -f")
      os.popen("find "+FOLDER+" -name \".scon*\" | xargs rm -rf")
      os.popen("find "+FOLDER+" -name \"kdiss*-data\" | xargs rm -rf")
      os.popen("find "+FOLDER+" -name \"*.pyc\" | xargs rm -f")
      os.popen("find "+FOLDER+" -name \"*.cache.py\" | xargs rm -f")
      os.popen("find "+FOLDER+" -name \"*.log\" | xargs rm -f")
      os.popen("find "+FOLDER+" -name \"*.kdevelop.*\" | xargs rm -f")
      os.popen("find "+FOLDER+" -name \"*~\" | xargs rm -f")

      os.popen("rm -rf "+FOLDER+"/autopackage")
      os.popen("rm -rf "+FOLDER+"/build")
      os.popen("rm -rf "+FOLDER+"/cache")
      os.popen("rm -f " +FOLDER+"/codeine-*.tar.bz2")
      os.popen("rm -f " +FOLDER+"/config.py*")
      os.popen("rm -f " +FOLDER+"/src/configure.h")
      os.popen("rm -f " +FOLDER+"/Doxyfile")
      os.popen("rm -f " +FOLDER+"/Makefile")
      os.popen("rm -rf "+FOLDER+"/packages")
      os.popen("rm -rf "+FOLDER+"/screenshots")
      os.popen("rm -f " +FOLDER+"/scons/signatures.dblite")

      ## make the tarball
      print GREEN+"Writing archive "+ARCHIVE+NORMAL
      os.popen("tar cjf "+ARCHIVE+" "+FOLDER)

      ## remove the temporary directory
      if os.path.isdir(FOLDER):
            shutil.rmtree(FOLDER)

      env.Default(None)
      env.Exit(0)
