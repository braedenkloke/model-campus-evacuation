import subprocess

PARKING_LOT_SCHEDULES_DIR = 'input_data/parking_lot_schedules'
RAW_OUTPUT_DATA_DIR = 'output_data/raw/'

def name_scenario():
    return input('Enter scenario name: ')

def configure_scenario():
    # Throw error if something goes wrong
    pass

def run_scenario(name):
    """
    Runs scenario with the given name.
    """
    args = ('ls')
    #args = ('./bin/campus_evacuation', '-i', PARKING_LOT_SCHEDULES_DIR + name + '.csv', 
            #'-o', RAW_OUTPUT_DATA_DIR + name '_log.csv')
    p = subprocess.run(args) 
    p.check_returncode() # Throws error if return code non-zero

def analyze_scenario():
    pass

def execute_script():
    name = name_scenario()
    configure_scenario()
    run_scenario(name)
    analyze_scenario()

if __name__ == '__main__':
    execute_script()
