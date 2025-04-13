import ladr.mace4 as mace4

class Mace4:
    def __init__(self):
        self.options = mace4.MaceOptions()
        mace4.init_mace_options(self.options)
        self.stats = mace4.MaceStats()
        self.results = None

    def set_options(self, **kwargs):
        for key, value in kwargs.items():
            if hasattr(self.options, key):
                setattr(self.options, key, value)

    def run(self, clauses):
        self.results = mace4.mace4(clauses, self.options)
        return self.results

    def get_results(self):
        return self.results

    def get_stats(self):
        return self.stats

    def exit(self, exit_code):
        mace4.mace4_exit(exit_code)
