#!/usr/bin/env python
"""Configure the build.

- Fetch HDF5 headers.
- Create libconfig.h
- Create defines.mk

This is not used by './unittest/'.
"""
import commands
import contextlib
import os
import sys

thisdir = os.path.dirname(os.path.abspath(__file__))

def log(msg):
    sys.stderr.write(msg)
    sys.stderr.write('\n')

def shell(cmd):
    log(cmd)
    status, output = commands.getstatusoutput(cmd)
    if status:
        raise Exception('%d <- %r' %(status, cmd))
    return output

def update_content(fn, content):
    direc = os.path.abspath(os.path.dirname(fn))
    if not os.path.isdir(direc):
        shell('mkdir -p %s' %direc)
    current_content = open(fn).read() if os.path.exists(fn) else None
    if content != current_content:
        log('writing to %r' %fn)
        log('"""\n' + content + '"""')
        open(fn, 'w').write(content)

def compose_libconfig(pbbam=False):
    if pbbam:
        content = """
#define USE_PBBAM
"""
    else:
        content = """
"""
    return content

def compose_defines_with_hdf(HDF5_INCLUDE, HDF5_LIB):
    """We have to use := for HDF5_LIB b/c blasr
    is using it to mean the directory, not the file,
    and it's in the environment.
    """
    return """
HDF5_INCLUDE:=%(HDF5_INCLUDE)s
HDF5_LIB:=%(HDF5_LIB)s
#CPPFLAGS+= -I../pbdata -I../hdf -I../alignment
LIBPBDATA_INCLUDE     ?=../pbdata
LIBPBIHDF_INCLUDE     ?=../hdf
LIBBLASR_INCLUDE      ?=../alignment
LIBPBDATA_LIB     ?=../pbdata/libpbdata.so
LIBPBIHDF_LIB     ?=../hdf/libpbihdf.so
LIBBLASR_LIB      ?=../alignment/libblasr.so
"""%(dict(
    thisdir=thisdir,
    HDF5_INCLUDE=HDF5_INCLUDE,
    HDF5_LIB=HDF5_LIB))


def compose_defines_with_hdf_headers(HDF_HEADERS):
    return """
HDF_HEADERS:=%(HDF_HEADERS)s
#HDF5_INC  ?=${HDF_HEADERS}/src
CPPFLAGS+= -I${HDF_HEADERS}/src -I${HDF_HEADERS}/c++/src
CPPFLAGS+= -I../pbdata -I../hdf -I../alignment
LIBPBDATA_LIB     ?=../pbdata/
LIBPBIHDF_LIB     ?=../hdf/
LIBBLASR_LIB      ?=../alignment/
"""%(dict(thisdir=thisdir, HDF_HEADERS=HDF_HEADERS))

def compose_defines():
    """
    Note that our local 'hdf' subdir will not even build
    in this case.
    """
    return """
LIBPBDATA_INC ?=../pbdata
LIBPBIHDF_INC ?=../hdf
LIBBLASR_INC  ?=../alignment
LIBPBDATA_LIB ?=%(thisdir)s/pbdata/
LIBPBIHDF_LIB ?=%(thisdir)s/hdf/
LIBBLASR_LIB  ?=%(thisdir)s/alignment/
nohdf         ?=1
"""%(dict(thisdir=thisdir))

def get_OS_STRING():
    G_BUILDOS_CMD = """bash -c 'set -e; set -o pipefail; id=$(lsb_release -si | tr "[:upper:]" "[:lower:]"); rel=$(lsb_release -sr); case $id in ubuntu) printf "$id-%04d\n" ${rel/./};; centos) echo "$id-${rel%%.*}";; *) echo "$id-$rel";; esac' 2>/dev/null"""
    return shell(G_BUILDOS_CMD)

def get_PBBAM(env, prefix):
    """
    key = 'PBBAM'
    if key in env:
        return env[key]
    cmd = 'cd $(THIRD_PARTY_PREFIX)/../staging/PostPrimary/pbbam 2>/dev/null && pwd || echo -n notfound' %(
            THIRD_PARTY_PREFIX=prefix)
    return shell(cmd)
    """
def get_HTSLIB(env, prefix):
    """
    key = 'HTSLIB'
    if key in env:
        return env[key]
    cmd = 'cd $(THIRD_PARTY_PREFIX)/../staging/PostPrimary/htslib 2>/dev/null && pwd || echo -n notfound' %(
            THIRD_PARTY_PREFIX=prefix)
    return shell(cmd)
    """
def ifenvf(env, key, func):
    if key in env:
        return env[key]
    else:
        return func()
def setifenvf(envout, envin, key, func):
    envout[key] = ifenvf(envin, key, func)
def setifenv(envout, envin, key, val):
    envout[key] = envin.get(key, val)
def setenv(envout, key, val):
    envout[key] = val
def update_env_if(envout, envin, keys):
    for key in keys:
        if key in envin:
            envout[key] = envin[key]
def compose_defs_env(env):
    # We disallow env overrides for anything with a default from GNU make.
    nons = ['CXX', 'CC', 'AR'] # 'SHELL'?
    ovr    = ['%-20s ?= %s' %(k, v) for k,v in env.items() if k not in nons]
    nonovr = ['%-20s := %s' %(k, v) for k,v in env.items() if k in nons]
    return '\n'.join(ovr + nonovr + [''])
def append_common(env, content):
    """Dumb way to do this, but this whole thing is evolving.
    """
    # Look into original configure dir for libconfig.h always.
    content += """
CPPFLAGS      += -I%s
"""%os.getcwd()
    # Some extra defs.
    reqs = ['SH_LIB_EXT',]
    vals = ['%-20s := %s' %(k, v) for k,v in env.items() if k in reqs]
    return content + '\n'.join([''] + vals + [''])
def compose_defines_pacbio(envin):
    """
    This is used by mobs via buildcntl.sh.
    """
    env = dict()
    setenv(env, 'SHELL', 'bash')
    setifenvf(env, envin, 'OS_STRING', get_OS_STRING)
    setifenv(env, envin, 'LIBPBDATA_INC', '../pbdata')
    setifenv(env, envin, 'LIBPBIHDF_INC', '../hdf')
    setifenv(env, envin, 'LIBBLASR_INC', '../alignment')
    setifenv(env, envin, 'LIBPBDATA_LIB', '../pbdata/')
    setifenv(env, envin, 'LIBPBIHDF_LIB', '../hdf/')
    setifenv(env, envin, 'LIBBLASR_LIB', '../alignment/')
    if 'nohdf' in envin:
        env['nohdf'] = envin['nohdf']
        # Otherwise, do not define it at all.
    nondefaults = set([
            'CXX', 'AR',
            'HDF5_INC', 'HDF5_LIB',
            'PBBAM_INC', 'PBBAM_LIB',
            'HTSLIB_INC', 'HTSLIB_LIB',
            'BOOST_INC',
            'ZLIB_LIB',
            'GCC_LIB',
            'GTEST_INC', 'GTEST_SRCDIR',
    ])
    update_env_if(env, envin, nondefaults)
    return compose_defs_env(env)

@contextlib.contextmanager
def cd(nwd):
    cwd = os.getcwd()
    log('cd %r -> %r' %(cwd, nwd))
    os.chdir(nwd)
    yield
    os.chdir(cwd)
    log('cd %r <- %r' %(cwd, nwd))

def fetch_hdf5_headers():
    """Fetch into ./hdf/HEADERS directory.
    This should not be used when an external build-dir is needed.
    Return actual directory path, relative to subdirs.
    """
    version = 'hdf5-1.8.12-headers'
    version_dn = os.path.join(thisdir, 'hdf', version)
    if not os.path.isdir(version_dn):
        with cd(os.path.dirname(version_dn)):
            cmd = 'curl -k -L https://www.dropbox.com/s/8971bcyy5o42rxb/hdf5-1.8.12-headers.tar.bz2\?dl\=0 | tar xjf -'
            shell(cmd)
    return version_dn # Relative path might help caching.

def update(content_defines_mk, content_libconfig_h):
    """ Write these relative to the same directory as *this* file.
    """
    fn_defines_mk = 'defines.mk'
    update_content(fn_defines_mk, content_defines_mk)
    fn_libconfig_h = os.path.join('.', 'libconfig.h')
    update_content(fn_libconfig_h, content_libconfig_h)
    if thisdir == os.path.abspath('.'):
        # This was run in the root directory, so symlink defines.mk
        # in sub-dirs, which now include defines.mk from CURDIR
        # in order to facilitate building in external output directories.
        for sub in ('pbdata', 'hdf', 'alignment', 'unittest'):
            lname = os.path.join(sub, 'defines.mk')
            if not os.path.lexists(lname):
                os.symlink(os.path.join('..', 'defines.mk'), lname)

def configure_nopbbam():
    """Use HDF5 from env-vars.
    This is the path used by blasr in a GitHub build, for now.
    """
    HDF5_INCLUDE = os.environ.get('HDF5_INCLUDE')
    if not HDF5_INCLUDE:
        HDF5_INCLUDE = os.environ['HDF5_INC']
    HDF5_LIB = os.environ['HDF5_LIB']
    content1 = compose_defines_with_hdf(HDF5_INCLUDE, HDF5_LIB)
    content1 = append_common(os.environ, content1)
    content2 = compose_libconfig(pbbam=False)
    update(content1, content2)

def configure_nopbbam_skip_hdf():
    """Fetch HDF5 headers.
    We lack HDF5 libs, so we cannot build our hdf/ subdir.
    But the others are fine.
    """
    HDF_HEADERS = fetch_hdf5_headers()
    content1 = compose_defines_with_hdf_headers(HDF_HEADERS)
    content1 = append_common(os.environ, content1)
    content2 = compose_libconfig(pbbam=False)
    update(content1, content2)

def configure_nopbbam_nohdf5():
    content1 = compose_defines()
    content1 = append_common(os.environ, content1)
    content2 = compose_libconfig(pbbam=False)
    update(content1, content2)

def configure_pacbio(envin):
    content1 = compose_defines_pacbio(envin)
    content1 = append_common(envin, content1)
    content2 = compose_libconfig(pbbam=True)
    update(content1, content2)

def get_make_style_env(envin, args):
    envout = dict()
    for arg in args:
        if '=' in arg:
            k, v = arg.split('=')
            envout[k] = v
    envout.update(envin)
    return envout

class OsType:
    Unknown, Linux, Darwin = range(3)

def getOsType():
    uname = shell('uname -s')
    log('uname=%r' %uname)
    if 'Darwin' in uname:
        return OsType.Darwin
    elif 'Linux' in uname:
        return OsType.Linux
    else:
        return OsType.Unknown

def update_env_for_linux(env):
    env['SET_LIB_NAME'] = '-soname'
    env['SH_LIB_EXT'] = '.so'
def update_env_for_darwin(env):
    env['SET_LIB_NAME'] = '-install_name'
    env['SH_LIB_EXT'] = '.dylib'
def update_env_for_unknown(env):
    env['SET_LIB_NAME'] = '-soname'
    env['SH_LIB_EXT'] = '.so'
update_env_for_os = {
    OsType.Linux: update_env_for_linux,
    OsType.Darwin: update_env_for_darwin,
    OsType.Unknown: update_env_for_unknown,
}

def main(prog, *args):
    """We are still deciding what env-vars to use, if any.
    """
    ost = getOsType()
    update_env_for_os[ost](os.environ)
    if 'NOPBBAM' in os.environ:
        if 'NOHDF' in os.environ:
            configure_nopbbam_nohdf5()
        else:
            if 'HDF5_LIB' in os.environ:
                assert 'HDF5_INC' in os.environ or 'HDF5_INCLUDE' in os.environ, 'Hey! You have HDF5_LIB but not HDF5_INCLUDE!'
                if 'HDF5_INC' not in os.environ:
                    print "NOT1"
                if 'HDF5_INCLUDE' not in os.environ:
                    print "NOT2"
                configure_nopbbam()
            else:
                configure_nopbbam_skip_hdf()
    else:
        envin = get_make_style_env(os.environ, args)
        configure_pacbio(envin)


if __name__=="__main__":
    main(*sys.argv)