#!/usr/bin/python
# -*- coding: UTF-8 -*-

# Copyright (C) 2010-2011 Ulteo SAS
# http://www.ulteo.com
# Author David LECHEVALIER <david@ulteo.com> 2011
# Author Julien LANGLOIS <julien@ulteo.com> 2010, 2011
# Author Samuel BOVEE    <samuel@ulteo.com> 2010
# Author Thomas MOUTON <thomas@ulteo.com> 2011
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 2
# of the License
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

import os
import sys
from distutils.core import setup

setup_args = {}
setup_args["name"] = "ovd-slaveserver"
setup_args["description"] = "OVD slaveserver"
setup_args["author"] = "Julien Langlois"
setup_args["author_email"] = "julien@ulteo.com"
setup_args["url"] = "http://www.ulteo.com/"
setup_args["version"] = "3.0.4.0"

"""
setup()
"""

if sys.platform == "win32":
	import py2exe
	sys.argv.append("py2exe")
	# Force the include of ApS because of dynamic imports
	sys.argv.append("-p")
	sys.argv.append("ovd.Role.ApplicationServer")

	try:
		import modulefinder
		import win32com,sys
		for p in win32com.__path__[1:]:
			modulefinder.AddPackagePath("win32com", p)
		for extra in ["win32com.shell","win32com.mapi"]:
			__import__(extra)
			m = sys.modules[extra]
			for p in m.__path__[1:]:
				modulefinder.AddPackagePath(extra, p)

	except ImportError:
		# no build path setup, no worries.
		pass
	
	class ServiceTarget:
		def __init__(self):
			self.description = "Ulteo OVD service"
			self.modules = ["OVDWin32Service"]
			self.company_name = "Ulteo SAS"
			self.copyright = "Copyright (C) 2015 Ulteo SAS"
			self.name = "Ulteo OVD"
			self.icon_resources = [(0, "media\\ulteo.ico")]
	
	class ConsoleTarget:
		def __init__(self):
			self.script = "ulteo-ovd-slaveserver"
			self.description = "Ulteo OVD alternate service"
			self.company_name = "Ulteo SAS"
			self.copyright = "Copyright (C) 2015 Ulteo SAS"
			self.name = "Ulteo OVD"
			self.icon_resources = [(0, "media\\ulteo.ico")]

	setup_args["zipfile"] = "lib/shared.zip"
	setup_args["console"] = [ConsoleTarget()]
	setup_args["service"] = [ServiceTarget()]
	setup_args["options"] = {"py2exe": {"dll_excludes": ["w9xpopen.exe"]}}
	


elif sys.platform == "linux2":
	setup_args["packages"] = [
		'ovd', 'ovd.Communication', 'ovd.Role',
		'ovd.Role.FileServer', 'ovd.Role.ApplicationServer',
		'ovd.Role.ApplicationServer.Platform',
		'ovd.Role.ApplicationServer.Platform.Linux',
		'ovd.Platform', 'ovd.Platform.Linux']



	setup_args["data_files"] = [
		('/usr/bin',['ulteo-ovd-slaveserver', 'ovd-slaveserver-role']),
		('/etc/ulteo/ovd',['examples/slaveserver.conf']),
		('/usr/share/ulteo/ovd/slaveserver',
			['examples/samba.conf', 'examples/apache2.conf'])]


else:
	raise Exception("No supported platform")

if __name__ == "__main__":
	setup(**setup_args)
