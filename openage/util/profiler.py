import cProfile, pstats, io
class Profiler:
	'''
		A class for quick and easy profiling.
		Usage:
			p = Profiler()
			with p:
				#call methods that need to be profiled
			p.report()
			p.close_stream()
	'''
	
    profile = None
    profile_stats = None
    profile_stream = None
    def __init__(self, oStream = io.StringIO()):
    	#oStream can be a file if the profile results want to be saved 
        self.profile = cProfile.Profile()
        self.profile_stream = oStream                        

	def __enter__(self):
		#begins profiling
		self.profile.clear()
		self.profile.enable()
		
	def __exit__(self, exc_type, exc_value, traceback):
		#stops profiling and saves resulsts into profile_stats
		self.profile.disable()
		self.profile_stats = pstats.Stats(self.profile, stream = self.profile_stream)
		self.change_ordering()
		
	def change_ordering(self, new_ordering = 'cumulative'):
		#change how the stats are sorted
		self.profile_stats.sort_stats(new_ordering)
		
	def report(self):
		#If profile_stream is a file, profile results are written to the file.
		#Otherwise, they are printed to the console.
		self.profile_stats.print_stats()
		if(isinstance(self.profile_stream, io.StringIO)):
			print(self.profile_stream.getvalue())
			
	def close_stream(self):
		self.profile_stream.close()

