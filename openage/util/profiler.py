import cProfile, pstats, io
class Profiler:
    '''
        A class for quick and easy profiling.
        Usage:
            p = Profiler()
            with p:
                #Call methods that need to be profiled.
            p.report()
    '''
    
    profile = None
    profile_stats = None
    profile_stream = None
    
    def __init__(self, oStream = None):
        #oStream can be a file if the profile results want to be saved.
        self.profile = cProfile.Profile()
        self.profile_stream = oStream                        

    def __enter__(self):
        #begins profiling
        self.profile.clear()
        self.profile.enable()
        
    def __exit__(self, exc_type, exc_value, traceback):
        #Stops profiling and saves results into profile_stats.
        self.profile.disable()

    def write_report(self, sortby = 'calls'):
        #Write the profile stats to profile_stream's file.
        self.profile_stats = pstats.Stats(self.profile, stream = self.profile_stream)
        self.profile_stats.sort_stats(sortby)
        self.profile_stats.print_stats()

    def report(self, sortby = 'calls'):
        #Return the profile_stats to the console.
        self.profile_stats = pstats.Stats(self.profile, stream = io.StringIO())
        self.profile_stats.sort_stats(sortby)
        self.profile_stats.print_stats()
        return self.profile_stats.stream.getvalue()



