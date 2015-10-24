Name:	    app-svc
Summary:    App svc
Version:    0.1.78
Release:    1
VCS:        magnolia/framework/appfw/app-svc#app-svc_0.1.48-29-gb33704a1b65d6dcdaba7d7da4c2770dbce0338dd
Group:      System/Libraries
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig


BuildRequires: cmake

BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(ecore-x)

%description
App svc

%package devel
Summary:    App svc
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}
%description devel
App svc (developement files)

%prep
%setup -q


%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif
%if 0%{?model_build_feature_appfw_multi_instance}
_APPFW_FEATURE_MULTI_INSTANCE=ON
%endif
%if 0%{?model_build_feature_appfw_multiwindow}
_APPFW_FEATURE_MULTI_WINDOW=ON
%endif

export CFLAGS="$CFLAGS -Wall -Werror -Wno-unused-function"
CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS" cmake . -DCMAKE_INSTALL_PREFIX=/usr \
	-D_APPFW_FEATURE_MULTI_INSTANCE:BOOL=${_APPFW_FEATURE_MULTI_INSTANCE} \
	-D_APPFW_FEATURE_MULTI_WINDOW:BOOL=${_APPFW_FEATURE_MULTI_WINDOW}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

%files
%manifest app-svc.manifest
%defattr(-,root,root,-)
/opt/usr/devel/appsvc_test
/usr/lib/libappsvc.so.0
/usr/lib/libappsvc.so.0.1.0
/usr/share/license/%{name}

%files devel
%defattr(-,root,root,-)
/usr/lib/pkgconfig/appsvc.pc
/usr/lib/libappsvc.so
/usr/include/appsvc/appsvc.h


