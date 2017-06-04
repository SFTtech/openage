import cProfile, pstats, io
class Profiler:
    profile = None
    profile_stats = None
    profile_stream = None
    stats = None
    def __init__(self):
        self.profile = cProfile.Profile()
                                
    def __enter__(self):
        if(self.profile_stream == None):
            self.profile_stream = io.StringIO()
        self.profile.clear()
        self.profile.enable()
        
    def __exit__(self, exc_type, exc_value, traceback):
        self.profile.disable()
        self.profile_stats = pstats.Stats(self.profile, stream = self.profile_stream)
        self.change_ordering('cumulative')
        
    def change_ordering(self, new_ordering):
        self.profile_stats.sort_stats(new_ordering)
        
    def report(self):
        self.profile.print_stats()
        self.profile_stream.close()
        #self.profile_stream = None
    
p = Profiler()
with p:
    print('cupcake')
p.profile_stats.sort_stats('cumulative')
p.report()
with p:
    print('zero')
p.change_ordering("calls")
p.report()
p.change_ordering('cumulative')
p.report()