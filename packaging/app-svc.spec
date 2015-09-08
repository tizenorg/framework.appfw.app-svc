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
BuildRequires: pkgconfig(ecore) 
BuildRequires: pkgconfig(ecore-x) 
BuildRequires: pkgconfig(x11)
BuildRequires: pkgconfig(libprivilege-control)
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(dbus-glib-1)
BuildRequires: pkgconfig(xdgmime)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(iniparser)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(vconf)

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

mkdir -p %{buildroot}/usr/share/appsvc
cp -R %{_builddir}/%{name}-%{version}/res/arm/usr/share/appsvc/* %{buildroot}/usr/share/appsvc

%post

/sbin/ldconfig
mkdir -p /opt/dbspace
sqlite3 /opt/dbspace/.appsvc.db < /opt/share/appsvc_db.sql
rm -rf /opt/share/appsvc_db.sql

chown 0:5000 /opt/dbspace/.appsvc.db
chown 0:5000 /opt/dbspace/.appsvc.db-journal
chmod 664 /opt/dbspace/.appsvc.db
chmod 664 /opt/dbspace/.appsvc.db-journal
chsmack -a 'app-svc::db' /opt/dbspace/.appsvc.db
chsmack -a 'app-svc::db' /opt/dbspace/.appsvc.db-journal

%postun -p /sbin/ldconfig

%files
%manifest app-svc.manifest
%defattr(-,root,root,-)
/opt/share/appsvc_db.sql
/opt/usr/devel/appsvc_test
/usr/lib/libappsvc.so.0
/usr/lib/libappsvc.so.0.1.0
/usr/share/license/%{name}
/usr/share/appsvc/*


%files devel
%defattr(-,root,root,-)
/usr/lib/pkgconfig/appsvc.pc
/usr/lib/libappsvc.so
/usr/include/appsvc/appsvc.h


