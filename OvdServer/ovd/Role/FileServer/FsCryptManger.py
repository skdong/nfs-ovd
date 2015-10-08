#!/usr/bin/env python

import commands
import os
import time

from ovd.Logger import Logger

class FsCryptManger:
	def __init__(self, directory, key_id):
		self.safe_dir = directory 
		self.key_id = key_id
		self.postfix = '.asc'

	def encrypt_dir(self):
		safe_files = self.traverse_path()
		for filepath in safe_files:
			if not filepath.endswith(self.postfix):
				self.encrypt_file(filepath)

	def decrypt_dir(self):
		safe_files = self.traverse_path()
		for filepath in safe_files:
			if filepath.endswith(self.postfix):
				self.decrypt_file(filepath)

	def traverse_path(self):
		safe_files = []
		for dirpath, dirnames, filenames in os.walk(self.safe_dir):
			for filename in filenames:
				filepath = os.path.join(dirpath,filename)
				if os.path.isfile(filepath) and '/.' not in filepath:
					safe_files.append(filepath)
		return safe_files

	def decrypt_file(self,path):
		if path.endswith(self.postfix) is False:
			Logger.error('the path is not a secret file %s'%(path))
			return None
		if path is None or len(path.strip()) is 0:
			Logger.error('the path is None or empty')
			return None
		if os.path.isfile(path) is False:
			Logger.error( 'the path is not file %s'%(path))
			return None

		dist_path = path[0:-4]
		decrypt_cmd = 'gpg -o %s --decrypt %s'%(dist_path, path)
		s,o = commands.getstatusoutput(decrypt_cmd)
		if s is not 0:
			Logger.error( 'decrypt path err cmd[%s]'%(decrypt_cmd))
			Logger.error( 'the out put [%d %s]'%(s,o))
			return None
		owner_cmd = 'chown --reference=%s %s'%(path, dist_path)
		s,o = commands.getstatusoutput(owner_cmd)
		if s is not 0:
			Logger.error( 'chown err cmd[%s]'%(owner_cmd))
			Logger.error( 'the out put [%d %s]'%(s,o))
			return None
		mode_cmd = 'chmod --reference=%s %s'%(path, dist_path)
		s,o = commands.getstatusoutput(mode_cmd)
		if s is not 0:
			Logger.error( 'chmod err cmd[%s]'%(mode_cmd))
			Logger.error( 'the out put [%d %s]'%(s,o))
			return None
		touch_cmd = 'touch --reference=%s %s'%(path, dist_path)
		s,o = commands.getstatusoutput(touch_cmd)
		if s is not 0:
			Logger.error( 'touch err cmd[%s]'%(touch_cmd))
			Logger.error( 'the out put [%d %s]'%(s,o))
			return None
		os.unlink(path)
		return True

	def encrypt_file(self,path):
		if path.endswith(self.postfix):
			Logger.error( 'the path is already encrypt')
			return None
		if path is None or len(path.strip()) is 0:
			Logger.error( 'the path is None or empty')
			return None
		if os.path.isfile(path) is False:
			Logger.error( 'the path is not file %s'%(path))
			return None

		encrypt_cmd = 'gpg -ea -r %s %s'%(self.key_id, path)
		s,o = commands.getstatusoutput(encrypt_cmd)
		if s is not 0:
			Logger.error( 'encrypt path err cmd[%s]'%(encrypt_cmd))
			Logger.error( 'the out put [%d %s]'%(s,o))
			return None
		owner_cmd = 'chown --reference=%s %s%s'%(path, path, self.postfix)
		s,o = commands.getstatusoutput(owner_cmd)
		if s is not 0:
			Logger.error( 'owner path err cmd[%s]'%(owner_cmd))
			Logger.error( 'the out put [%d %s]'%(s,o))
			return None
		mode_cmd = 'chmod --reference=%s %s%s'%(path, path, self.postfix)
		s,o = commands.getstatusoutput(mode_cmd)
		if s is not 0:
			Logger.error( 'mode path err cmd[%s]'%(mode_cmd))
			Logger.error( 'the out put [%d %s]'%(s,o))
			return None
		touch_cmd = 'touch --reference=%s %s%s'%(path, path, self.postfix)
		s,o = commands.getstatusoutput(touch_cmd)
		if s is not 0:
			Logger.error( 'touch path err cmd[%s]'%(touch_cmd))
			Logger.error( 'the out put [%d %s]'%(s,o))
			return None

		os.unlink(path)
		return True

if __name__ == '__main__':
	crypt_m = FsCryptManger('/root/test/ulteo-ovd-sources-3.0.4','nfstest')
	print crypt_m.safe_dir
	time_str = '%d %d'%(time.localtime().tm_min,time.localtime().tm_sec)
	print time_str
	#crypt_m.encrypt_dir()
	crypt_m.decrypt_dir()
	time_str = '%d %d'%(time.localtime().tm_min,time.localtime().tm_sec)
	print time_str
	#if crypt_m.decrypt_file('/home/nfs/test/xrdp/configure.ac.asc') is not None:
	#	print 'ok'
	#crypt_m.traverse_path()
	#for filepath in crypt_m.safe_files:
	#	print filepath
