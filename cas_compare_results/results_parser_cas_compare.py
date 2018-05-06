import re
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
import math

RESULTS_LINES = 4
PARAMS = ['noperations', 'nthreads']
def parse_single_result(result):

    splitted_result = result.split('\x0a')
    return {"noperations": splitted_result[0].replace('noperations: ', ''),
            "nthreads":  splitted_result[1].replace('nthreads: ', ''),
            "htm_cas_result":  splitted_result[2].replace('htm cas: ', ''),
            "std_cas_result":  splitted_result[3].replace('std cas: ', ''),
            }


def parse_file(path):
    f = open(path, 'r')
    parsed = []
    tmp = ""
    i = 1
    for line in f:
        tmp += line
        if i % RESULTS_LINES == 0:
            parsed.append(parse_single_result(tmp))
            tmp = ""
        i += 1
    f.close()
    return parsed


def to_plot(results, changing_param_name, with_log_x=False, with_log_y=False):
    constant_params = [p for p in PARAMS if p != changing_param_name]
    title = "By {}, ".format(changing_param_name)
    for p in constant_params:
        title += "{} = {} ".format(p, results[0][p]) 
    x_axis = [float(r[changing_param_name]) for r in results]
    _to_plot(results, x_axis, changing_param_name, title, with_log_x, with_log_y)

def _to_plot(results, x_axis, x_text, title, with_log_x, with_log_y):
    y_text = 'Microseconds to complete'
    htm_result = [float(r['htm_cas_result']) for r in results]
    std_result = [float(r['std_cas_result']) for r in results]
    if with_log_y:
        htm_result = [math.log(y) for y in htm_result]
        std_result = [math.log(y) for y in std_result]
        y_text = "log(" + y_text + ")"
    if with_log_x:
        x_axis = [math.log(x) for x in x_axis]
        x_text = "log(" + x_text + ")"

    plt.title(title)
    plt.subplot(211)
    green_patch = mpatches.Patch(color='green', label='HTM CAS')
    blue_patch = mpatches.Patch(color='blue', label='STD CAS')
    plt.legend(handles=[green_patch, blue_patch])
    plt.title(title)
    plt.xlabel(x_text)
    plt.ylabel(y_text)
    plt.plot(x_axis, htm_result, 'g-o', x_axis, std_result, 'b-o')


if __name__ == '__main__':
    # plt.figure(1)
    # to_plot(parse_file('changing_noperations.txt'), 'noperations')

    # plt.figure(2)
    # to_plot(parse_file('changing_noperations.txt'), 'noperations', True, True)
    # plt.figure(3)
    # to_plot(parse_file('changing_noperations_morethreads.txt'), 'noperations', True)
    
    # plt.figure(4)
    # to_plot(parse_file('changing_noperations_morethreads.txt'), 'noperations', True, True)
    
    # plt.figure(5)
    # to_plot(parse_file('changing_threads.txt'), 'nthreads')

    plt.figure(6)
    to_plot(parse_file('changing_threads.txt'), 'nthreads', False, True)

    plt.figure(7)
    to_plot(parse_file('changing_noperations_1_thread.txt'), 'noperations')


    plt.show()