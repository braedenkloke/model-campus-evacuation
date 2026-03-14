import subprocess
import csv

PARKING_LOT_SCHEDULES_DIR = 'input_data/parking_lot_schedules/'
DEFAULT_PARKING_LOT_SCHEDULE = PARKING_LOT_SCHEDULES_DIR + 'default.csv'
RAW_OUTPUT_DATA_DIR = 'output_data/raw/'
PARKING_LOT_ID_INDEX = 0
DELAY_INDEX = 1
YES = 'y'

def name_scenario():
    name = input('What would you like to name the scenario?\n')
    return name.lower()


def configure_scenario(scenario_name):
    with open(DEFAULT_PARKING_LOT_SCHEDULE, newline='') as f:
        default_config = csv.reader(f, delimiter=',') 
        parking_lot_ids = list()
        col_names = list()
        config = list()
        for row in default_config:
            if len(col_names) == 0:
                col_names.append(row)
            else:
                parking_lot_ids.append(row[PARKING_LOT_ID_INDEX])
                config.append(row)


    cmd = YES
    while (cmd == YES):
        # Display current configuration
        config_header = '\t\t'.join(['ID', 'Delay', 'Period', 'Capacity'])
        print(config_header)
        for row in config:
            print('\t\t'.join(row))


        cmd = input(f'Would you like to change the configuration? [{YES}/n]\n')
        if cmd == YES:
            parking_lot_id = input(f'Which parking lot configuration would you like to change? {parking_lot_ids}\n')
            parking_lot_id = parking_lot_id.upper()
            if parking_lot_id in parking_lot_ids:
                delay_in_seconds = input(f'For {parking_lot_id}, how much should the evacuation be delayed? (seconds)\n')
                for row in config:
                    if row[PARKING_LOT_ID_INDEX] == parking_lot_id:
                        row[DELAY_INDEX] = delay_in_seconds
            else:
                print('Invalid parking lot ID')

    config_filepath = PARKING_LOT_SCHEDULES_DIR + scenario_name +'.csv'
    with open(config_filepath, 'w', newline='') as f:
        writer = csv.writer(f, delimiter=',')
        writer.writerow(col_names[0])
        for row in config:
            writer.writerow(row)
        print(f'Configuration saved: {config_filepath}')

def run_scenario(scenario_name):
    """
    Runs scenario with the given name.
    """
    cmd = input(f'Would you like to run a simulation of your scenario? [{YES}/n]\n')
    if cmd == YES:
        parking_lot_schedule_fp = PARKING_LOT_SCHEDULES_DIR + scenario_name + '.csv'
        log_fp = RAW_OUTPUT_DATA_DIR + scenario_name + '_log.csv'
        args = ('./bin/campus_evacuation', '-i', parking_lot_schedule_fp, '-o', log_fp)
        p = subprocess.run(args) 
        p.check_returncode() # Throws error if return code non-zero
        print(f'Simulation complete: {log_fp}\n')

def analyze_scenario():
    pass

def execute_script():
    name = name_scenario()
    configure_scenario(name)
    run_scenario(name)
    #analyze_scenario()

if __name__ == '__main__':
    execute_script()
