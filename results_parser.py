import re
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt

RESULTS_LINES = 5
PARAMS = ['noperations', 'nthreads', "ratio"]
def parse_single_result(result):
    pattern = r"""Running (?P<noperations>\d+) operations on (?P<nthreads>\d+) threads, (?P<trials>\d+) trials  with ratio (?P<ratio>([0-9]*\.[0-9]+|[0-9]+))
total failures: (?P<std_cas_total_failures>\d+)
std cas: (?P<std_cas_result>\d+)
total failures: (?P<htm_cas_total_failures>\d+)
htm cas: (?P<htm_cas_result>\d+)
"""
    p = re.compile(pattern)
    m = p.match(result)
    return {"noperations": m.group("noperations"),
            "nthreads":  m.group("nthreads"),
            "trials":  m.group("trials"),
            "ratio": m.group("ratio"),
            "std_cas_total_failures":  m.group("std_cas_total_failures"),
            "std_cas_result":  m.group("std_cas_result"),
            "htm_cas_total_failures":  m.group("htm_cas_total_failures"),
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

def normalize_to_falires(results):
    for result in results:
        if result['std_cas_total_failures'] != '0':
            result['std_cas_result_normalized'] = int(result['std_cas_result']) / float(result['std_cas_total_failures'])
        else:
            result['std_cas_result_normalized'] = int(result['std_cas_result'])
        if result['htm_cas_total_failures'] != '0':
            result['htm_cas_result_normalized'] = int(result['htm_cas_result']) / float(result['htm_cas_total_failures'])
        else:
            result['htm_cas_result_normalized'] = int(result['htm_cas_result'])


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
    normalize_to_falires(results)
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


if __name__ == '__main__':
    plt.figure(1)
    to_plot(parse_file('changing_noperations.txt'), 'noperations')
    plt.figure(2)
    to_plot(parse_file('changing_noperations_morethreads.txt'), 'noperations')
    plt.figure(3)
    to_plot(parse_file('changing_nthreads.txt'), 'nthreads')
    plt.figure(4)
    to_plot(parse_file('changing_ratio.txt'), 'ratio')
    plt.figure(5)
    to_plot(parse_file('changing_ratio_morethreads.txt'), 'ratio')

    plt.figure(6)
    to_plot(parse_file('changing_noperations.txt'), 'noperations', True)
    plt.figure(7)
    to_plot(parse_file('changing_noperations_morethreads.txt'), 'noperations', True)
    plt.figure(8)
    to_plot(parse_file('changing_nthreads.txt'), 'nthreads', True)
    plt.figure(9)
    to_plot(parse_file('changing_ratio.txt'), 'ratio', True)
    plt.figure(10)
    to_plot(parse_file('changing_ratio_morethreads.txt'), 'ratio', True)
    plt.show()