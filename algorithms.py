import collections

algorithms = collections.OrderedDict((line.split()[0], line.split()[1:]) for line in open('algorithms').readlines())
