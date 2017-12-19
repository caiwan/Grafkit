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

    SLEEP = .25

    def __init__(self, src_dir, dst_dir):
        FileSystemEventHandler.__init__(self)
        self.src = src_dir
        self.dst = dst_dir
        self.lg = logging.getLogger('EvtHandler')
        pass
        
    def copy(self, evt):
        for i in range(3):
            try:
                src = evt.src_path.lower()
                if not os.path.isfile(src):
                    return
                    
                if src.endswith(".tmp") or src.lower().endswith("~"):
                    return
                    
                dst = self.dst + os.path.relpath(evt.src_path, start=self.src)
                shutil.copy(evt.src_path, dst)
                self.lg.info("Copy file "+evt.src_path+ " to " + dst)
                
                return
            except IOError as e:
                self.lg.error("fuk {}".format(str(e)))
                if e.errno == 2:
                    return
                time.sleep(self.SLEEP)
            pass
        
    def on_created(self, evt):
        time.sleep(self.SLEEP)
        self.copy(evt)
        
    def on_modified(self, evt):
        time.sleep(self.SLEEP)
        self.copy(evt)
    pass
    
    def on_moved(self, evt):
        time.sleep(self.SLEEP)
        self.copy(evt)
    pass
    
if __name__ == "__main__":

    if len(sys.argv) != 3:
        print_usage()
        sys.exit(0)

    input = sys.argv[1]
    output = sys.argv[2]
        
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')
                        
    event_handler_log = LoggingEventHandler()
    event_handler = EvtHandler(input, output)
    
    observer = Observer()
    observer.schedule(event_handler_log, input, recursive=True)
    observer.schedule(event_handler, input, recursive=True)
    observer.start()
    
    print ("Watching asset changes from now on")
    
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()
