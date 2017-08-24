import sys
import time
import logging
import os
import shutil
from watchdog.observers import Observer
from watchdog.events import LoggingEventHandler, FileSystemEventHandler

def print_usage():
    print('QnD File system watcher')
    print('- copies modified file to another dir')
    print('')
    print('usage: fs_watcher.py <input_dir> <output_file>')


class EvtHandler(FileSystemEventHandler): 
    def __init__(self, dst_dir):
        FileSystemEventHandler.__init__(self)
        self.dst = dst_dir
        self.lg = logging.getLogger('EvtHandler')
        pass
        
    def on_created(self, evt):
        try:
            shutil.copy(evt.src_path, self.dst)
            self.lg.info("Copy file " + evt.src_path)
        except:
            pass
        pass
        
    def on_modified(self, evt):
        try :
            shutil.copy(evt.src_path, self.dst)
            self.lg.info("Copy file " + evt.src_path)
        except:
            pass
        pass        
    pass
    
if __name__ == "__main__":

    if len(sys.argv) != 3:
        print_usage()
        sys.exit(0)

    input = sys.argv[1]
    output = sys.argv[2]
        
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')
                        
    event_handler_log = LoggingEventHandler()
    event_handler = EvtHandler(output)
    
    observer = Observer()
    observer.schedule(event_handler_log, input, recursive=True)
    observer.schedule(event_handler, input, recursive=True)
    observer.start()
    
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()