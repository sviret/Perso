execfile('src/load.py', globals()) # don't remove this!

#
# This scripts provides an example of MIB rate analysis for 
# a single CMS run
#
# SV (viret@in2p3.fr) : 15/11/10 (creation) 
#
# For more info, have a look at the CMS MIB webpage:
# 
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
#


#
# First give the number of the run you want to analyze 
# 
# You could find info on the run on the CMS WBM page:
#
# https://cmswbm.web.cern.ch/cmswbm/cmsdb/servlet/RunSummary?RUN=****
#
# where **** is the run number

# nsls /castor/cern.ch/user/s/sviret/CMS/MIB/DQ/Prod | sed ':start /^.*$/N;s/\n/','/g; t start' | sed 's/MIB_data_result_//g;s/.root//g;'


#runs = [136001,136080,136087,136088,136091,136092,136100,136119,136248,136255,136256,136259,136323,136330,136335,136441,137027,137525,137528,137605,137618,137619,137626,137631,137632,137651,137694,137701,137756,137766,137768,137772,137773,137960,138412,138426,138429,138442,138455,138489,138526,138572,138696,138747,138750,138751,138916,138919,139096,139103,139106,139110,139195,139196,139269,139272,139308,139356,139372,139375,139411,139664,139781,139965,140058,140073,140085,140116,140156,140159,140182,140331,140352,140361,140362,140379,140382,140383,140385,140399,140401,141865,141880,141881,141882,141887,141950,141952,142128,142266]


#runs = [138412,138426,138429,138442,138455,138489,138526,138572,138696,138739,138749,138750,138916,138919,138921,139096,139098,139103,139106,139110,139195,139196,139239,139269,139272,139308,139356,139368,139372,139375,139411,139778,139781,139889,139965,139971,139980,140058,140073,140085,140156,140182]

#runs = [138412,139356,139368,139372,139375]

runs = [138412,138426,138429,138442,138455,138489,138526,138572,138696,138749,138750,138916,138919,138921,139096,139098,139103,139106,139110,139195,139196,139269,139272,139308,139356,139368,139372,139375,139778,139965,139980]

#runs = [138412,138426,138429,138442,138455,138489,138526,138572,138696,138739,138747,138749,138750,138751,138916,138919,138921,139020,139096,139098,139100,139103,139106,139110,139195,139196,139239,139269,139272,139308,139347,139356,139365,139368,139370,139372,139375,139407,139411,139458,139664,139778,139781,139790,139889,139965,139971,139980,140058,140059,140073,140085,140116,140124,140156,140158,140159,140160,140182,140331,140352,140361,140362,140379,140382,140383,140385,140399,140401,141865,141880,141881,141882,141887,141950,141952,141956,141960,142035,142036,142038,142040,142076,142128,142130,142132,142135,142136,142137,142187,142189,142191,142265,142266,142303,142305,142311,142312,142313,142317,142320,142413,142414,142419,142420,142422,142523,142524,142525,142528,142537,142557,142558,142653,142662,142663,142666,142737,142762,142774,142851,142867,142928,142933,142953,142954,142955,142970,142971,142974,143004,143005,143007,143008,143179,143181,143187,143191,143318,143320,143322,143323,143326,143327,143328,143657,143665,143727,143731,143827,143833,143835,143953,143954,143961,143962,144008,144011,144083,144086,144089,144112,144114]

#runs = [138412,138426,138429,138442,138455,138489,138526,138572,138696,138739,138747,138749,138750,138751,138916,138919,138921,139020,139096,139098,139100,139103,139106,139110,139195,139196,139239,139269,139272,139308,139347,139356,139365,139368,139370,139372,139375,139407,139411,139458,139664,139778,139781,139790,139889,139965,139971,139980,140058,140059,140073,140085,140116,140124,140156,140158,140159,140160,140182,140331,140352,140361,140362,140379,140382,140383,140385,140399,140401,141865,141880,141881,141882,141887,141950,141952,141956,141960,142035,142036,142038,142040,142076,142128,142130,142132,142135,142136,142137,142187,142189,142191,142265,142266,142303,142305,142311,142312,142313,142317,142320,142413,142414,142419,142420,142422,142523,142524,142525,142528,142537,142557,142558,142653,142662,142663,142666,142737,142762,142774,142851,142867,142928,142933,142953,142954,142955,142970,142971,142974,143004,143005,143007,143008,143179,143181,143187,143191,143318,143320,143322,143323,143326,143327,143328,143657,143665,143727,143731,143827,143833,143835,143953,143954,143961,143962,144008,144011,144083,144086,144089,144112,144114,146325,146335,146436,146437,146510,146511,146514,146589,146644,146698,146710,146713,146715,146719,146724,146728,146804,146807,146944,147043,147048,147114,147115,147206,147217,147219,147222,147284,147289,147390,147450,147451,147754,147755,147757,147926,147927,147929,148002,148029,148031,148032,148064,148819,148829,148862,148864,148952,148953,149003,149181,149182,149291,149294,149378,149379,149442,149461,149509,150063,150064,150219,150238,150273,150277,150286,150308,150323,150329,150342,150375,150392,150405,150411,150414,150436,150442,150619,150844,151019,151268,151353,151476,151584,151586,151592,151706,151784,151923,151925,151927,151935,151940,151969,151975,152047,152110,152113,152267,152327,152349,152431,152471,152474,152477,152479,152494,152585,152592,152597,152601,152611,152629,152638,152648]


#
# Create the list of events to be analyzed, and read the ROOTuple information
#
#

a = Use(runs)
b = ReadMIBSummary()


# Launch the analysis

#processors = [a, b, d, f, g]
processors = [a,b]

#
# The last steps are describing the plot production (default plots are .png)
# 
# At each level, you could produce plots for few parts
# or for all the TileCal
#
# Below are some examples (uncomment what you want to try)

# Process all the monitoring plots
'''
for i in range(5):
    for j in range(12):
        processors.append(do_bit_trend_plot(bit=j+32,pixcut=i,doEps=True))

    processors.append(do_no34_trend_plot(pixcut=i,doEps=True))
'''
# Add the html page production

processors.append(do_bunches_trend_plot(doEps=True))

processors.append(do_prescale_trend_plot(doEps=True))

processors.append(do_trend_html())



#
# Go for it!!!
#

Go(processors)


