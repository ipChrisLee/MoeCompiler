import subprocess
from pathlib import Path
import shutil


class Bash:
	@staticmethod
	def file_copy(srcFilePath: str, dstFilePath: str):
		return shutil.copy(src=srcFilePath, dst=dstFilePath)
