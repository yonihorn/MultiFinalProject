import re
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
import os

RESULTS_LINES = 5
PARAMS = ['noperations', 'nthreads']
def parse_single_result(result):
    pattern = r"""Running (?P<noperations>\d+) operations (per thread) on (?P<nthreads>\d+) threads
htm cas: (?P<htm_cas_result>\d+)
std cas: (?P<std_cas_result>\d+)
"""
    p = re.compile(pattern)
    m = p.match(result)
    return {"noperations": m.group("noperations"),
            "nthreads":  m.group("nthreads"),
            "std_cas_result":  m.group("std_cas_result"),
            "htm_cas_result":  m.group("htm_cas_result")
            }


def parse_file(path):
    f = open(path, 'r')
    parsed = []
    tmp = ""
    i = 1
    for line in f:
        tmp += line
        if i% 5 == 0:
            parsed.append(parse_single_result(tmp))
            tmp = ""
        i += 1
    f.close()
    return parsed


def to_plot(results, changing_param_name, normalized=False):
    constant_params = [p for p in PARAMS if p != changing_param_name]
    title = "By {}, ".format(changing_param_name)
    for p in constant_params:
        title += "{} = {} ".format(p, results[0][p]) 
    x_axis = [float(r[changing_param_name]) for r in results]
    if normalized:
        _to_plot_normalized(results, x_axis, changing_param_name, title)
    else:
        _to_plot(results, x_axis, changing_param_name, title)

def _to_plot(results, x_axis, x_text, title):
    htm_result = [float(r['htm_cas_result']) for r in results]
    htm_failures = [float(r['htm_cas_total_failures']) for r in results]
    std_result = [float(r['std_cas_result']) for r in results]
    std_failures = [float(r['std_cas_total_failures']) for r in results]
    plt.title(title)
    plt.subplot(211)
    green_patch = mpatches.Patch(color='green', label='HTM CAS')
    blue_patch = mpatches.Patch(color='blue', label='STD CAS')
    plt.legend(handles=[green_patch, blue_patch])
    plt.title(title)
    plt.xlabel(x_text)
    plt.ylabel('Microseconds to complete')
    plt.plot(x_axis, htm_result, 'g', x_axis, std_result, 'b')
    plt.subplot(212)
    plt.legend(handles=[green_patch, blue_patch])
    plt.title(title)
    plt.xlabel(x_text)
    plt.ylabel('Number of CAS failures')
    plt.plot(x_axis, htm_failures, 'g--', x_axis, std_failures, 'b--')

def _to_plot_normalized(results, x_axis, x_text, title):
    htm_result = [float(r['htm_cas_result_normalized']) for r in results]
    std_result = [float(r['std_cas_result_normalized']) for r in results]
    plt.title(title)
    green_patch = mpatches.Patch(color='green', label='HTM CAS')
    blue_patch = mpatches.Patch(color='blue', label='STD CAS')
    plt.legend(handles=[green_patch, blue_patch])
    plt.title(title)
    plt.xlabel(x_text)
    plt.ylabel('Microseconds to complete - Normalized to failures')
    plt.plot(x_axis, htm_result, 'g', x_axis, std_result, 'b')


DIR = 'cas_compare_results'
if __name__ == '__main__':
    plt.figure(1)
    to_plot(parse_file(os.path.join(DIR, 'changing_noperations.txt')), 'noperations')
    plt.figure(2)
    to_plot(parse_file(os.path.join(DIR, 'changing_noperations_morethreads.txt')), 'noperations')
    plt.figure(3)
    to_plot(parse_file(os.path.join(DIR, 'changing_nthreads.txt')), 'nthreads')


    plt.figure(6)
    to_plot(parse_file(os.path.join(DIR, 'changing_noperations.txt')), 'noperations', True)
    plt.figure(7)
    to_plot(parse_file(os.path.join(DIR, 'changing_noperations_morethreads.txt')), 'noperations', True)
    plt.figure(8)
    to_plot(parse_file(os.path.join(DIR, 'changing_nthreads.txt')), 'nthreads', True)

    plt.show()