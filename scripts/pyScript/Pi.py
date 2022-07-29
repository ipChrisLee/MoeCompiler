import typing as typ
import subprocess as sp
import paramiko
import os
import atexit
from pathlib import Path
from settings import TestFilesSettings, TimeoutSettings
import json


class Pi:
	piIP: str = '192.168.0.101'
	piUserName = 'pi'
	pi: str = f'{piUserName}@{piIP}'
	projPathOnPi: str = 'Proj/MoeCompiler'
	piTestInFilePath: str = f'{projPathOnPi}/test.in'
	piTestOutFilePath: str = f'{projPathOnPi}/test.out'
	piTestSFilePath: str = f'{projPathOnPi}/test.s'
	piResultFilePath: str = f'{projPathOnPi}/result.json'
	piTestPyFilePath: str = f'{projPathOnPi}/test.py'
	piALibFilePath: str = f'{projPathOnPi}/libsysy.a'
	_piFiles: typ.Dict[str, str] = {
		'scripts/piScripts/test.py': f'{projPathOnPi}/test.py',
		'scripts/piScripts/runpy.sh': f'{projPathOnPi}/runpy.sh',
		'scripts/piScripts/compile_run.sh': f'{projPathOnPi}/compile_run.sh'
	}
	commandToRunPiTester: typ.List[str] = [
		'python', f'{piTestPyFilePath}',
		'--ms', f'{piTestSFilePath}',
		'--in', f'{piTestInFilePath}',
		'--out', f'{piTestOutFilePath}',
		'--res', f'{piResultFilePath}',
		'--alib', f'{piALibFilePath}',
	]
	piSSH = paramiko.SSHClient()
	
	@staticmethod
	def send_to_pi(
		localFilePath: str, piFilePath: typ.Optional[str] = None,
		piDirPath: typ.Optional[str] = None
	):
		if (piFilePath is None and piDirPath is None) or (
			piFilePath is not None and piDirPath is not None):
			print('argument error.')
			exit(-1)
		if not Path(localFilePath).exists():
			print(f'{localFilePath} not found!')
			exit(-1)
		
		if piDirPath is not None and piFilePath is None:
			piFilePath = \
				Path(f'{Pi.projPathOnPi}') / Path(f'{localFilePath}').name
			piFilePath = str(piFilePath)
		sftp = Pi.piSSH.open_sftp()
		sftp.put(localpath=localFilePath, remotepath=piFilePath)
		sftp.close()
	
	@staticmethod
	def get_from_pi(
		piFilePath: str, localFilePath: str
	):
		sftp = Pi.piSSH.open_sftp()
		sftp.get(localpath=localFilePath, remotepath=piFilePath)
		sftp.close()
	
	@staticmethod
	def _copy_necessary_files_to_pi():
		for localFilePath, piFilePath in Pi._piFiles.items():
			Pi.send_to_pi(localFilePath=localFilePath, piFilePath=piFilePath)
	
	@staticmethod
	def run(args: typ.List[str]):
		"""
		Blocking call commands. Kill this program if failed.
		"""
		stdin, stdout, stderr \
			= Pi.piSSH.exec_command(sp.list2cmdline(args),
			                        timeout=TimeoutSettings.run)
		exitStatus = stdout.channel.recv_exit_status()
		return {
			'exit_status': exitStatus,
			'info': stderr.readlines()
		}
	
	@staticmethod
	def run_tester(
		sFilePath: str, inFilePath: str, outFilePath: str, resFilePath: str
	):
		"""
		:param sFilePath: sy file path on local;
		:param inFilePath: in file path on local;
		:param outFilePath: out file path on local;
		:param resFilePath: file path to save res on local.
		:return res json
		"""
		Pi.send_to_pi(sFilePath, piFilePath=Pi.piTestSFilePath)
		Pi.send_to_pi(inFilePath, piFilePath=Pi.piTestInFilePath)
		Pi.send_to_pi(outFilePath, piFilePath=Pi.piTestOutFilePath)
		res = Pi.run(Pi.commandToRunPiTester)
		Pi.get_from_pi(
			piFilePath=Pi.piResultFilePath, localFilePath=resFilePath
		)
		if res['exit_status'] != 0:
			return res
		with open(resFilePath, 'r') as fp:
			res = json.load(fp)
		return res


Pi.piSSH.load_host_keys(
	os.path.expanduser(os.path.join("~", ".ssh", "known_hosts"))
)

Pi.piSSH.connect(
	hostname=Pi.piIP, port=22, username=Pi.piUserName
)
Pi._copy_necessary_files_to_pi()


def _close_all():
	Pi.piSSH.close()


atexit.register(_close_all)
