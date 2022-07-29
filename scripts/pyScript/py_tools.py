from color import C, cprint
import typing as typ


def exit_with_msg(exit_code: int = 0, msg: typ.AnyStr = 'Normal exit.'):
	if exit_code != 0:
		cprint(msg, color=C.ERR)
	else:
		cprint(msg, color=C.FIN)
	exit(exit_code)


class classproperty(property):
	def __get__(self, cls, owner):
		return classmethod(self.fget).__get__(None, owner)()
