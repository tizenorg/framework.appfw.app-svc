#sbs-git:slp/pkgs/a/app-svc app-svc 0.1.19 de68cdc468eaded317b4f5be9cdafebb01639aa0
Name:	    app-svc
Summary:    App svc
Version: 0.1.27
Release:    1
Group:      System/Libraries
License:    Apache License, Version 2.0
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig


BuildRequires: cmake

BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(ecore) 
BuildRequires: pkgconfig(x11)
BuildRequires: pkgconfig(libprivilege-control)
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(dbus-glib-1)
BuildRequires: pkgconfig(ail)
BuildRequires: pkgconfig(xdgmime)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(libsoup-2.4)


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

CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS" cmake . -DCMAKE_INSTALL_PREFIX=/usr

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install


%post

/sbin/ldconfig
mkdir -p /opt/dbspace
sqlite3 /opt/dbspace/.appsvc.db < /opt/share/appsvc_db.sql
rm -rf /opt/share/appsvc_db.sql

chown 0:5000 /opt/dbspace/.appsvc.db
chown 0:5000 /opt/dbspace/.appsvc.db-journal
chmod 664 /opt/dbspace/.appsvc.db
chmod 664 /opt/dbspace/.appsvc.db-journal

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
/opt/share/appsvc_db.sql
/usr/bin/appsvc_test
/usr/lib/libappsvc.so.0
/usr/lib/libappsvc.so.0.1.0

%files devel
%defattr(-,root,root,-)
/usr/lib/pkgconfig/appsvc.pc
/usr/lib/libappsvc.so
/usr/include/appsvc/appsvc.h


