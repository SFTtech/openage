import tempfile
import shutil
import os

class FileSystemAPI:
    def __init__(self):
        pass

    def create_temp_file(self):
        temp_file = tempfile.NamedTemporaryFile(delete=False)
        return temp_file.name

    def create_temp_dir(self):
        temp_dir = tempfile.mkdtemp()
        return temp_dir

    def delete_temp(self, path):
        if os.path.isfile(path):
            os.remove(path)
        elif os.path.isdir(path):
            shutil.rmtree(path)

    def use_temp_in_converter(self):
        temp_file_path = self.create_temp_file()
        print(f"Temporary file path created: {temp_file_path}")
     


fs = FileSystemAPI()
fs.use_temp_in_converter()
