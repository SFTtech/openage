class ExtractionRule:
	def __init__(self, rulestr):
		drsname, fname = rulestr.split(':')
		fnostr, fext = fname.split('.')

		if drsname == '*':
			drsname = None
		if fnostr == '*':
			fno = None
		else:
			fno = int(fnostr)
		if fext == '*':
			fext = None

		self.drsname = drsname
		self.fno = fno
		self.fext = fext

	def matches(self, drsname, fno, fext):
		if self.drsname != drsname and not self.drsname == None:
			return False
		if self.fno != fno and not self.fno == None:
			return False
		if self.fext != fext and not self.fext == None:
			return False

		return True
