# -*- mode: python -*-

# Inherit the environment from my parent.
Import('env')

ccflag = ['-Wall']

# Make a copy of the environment, so my changes are limited in this directory and sub-directories.
env = env.Clone()
env.Append(CPPPATH=['#include',
                    '#utility/mt_htmlparser',
                    '#utility/codedetector',
                    '#utility/wordbreaker',
                    '#utility/common',
                    '#utility/mt_parallel/',
                    '#component/metafeatureprocessor/common',
                    '#mining_system/maxent-20061005/src/'])


env.Append(LIBPATH=['/usr/local/lib'])
env.aProgram(target = 'mtParallelSentenceMining',
             source = ['mtParallelSentenceMining.cpp' ,
                       'mt_parallel_mining_task.cpp', 
                       'mt_sentence.cpp',
                       'mt_html_filter.cpp', 
                       'mt_corpus_extractor.cpp',
                       'mt_sentence_align.cpp',
                       'mt_bigraph_match.cpp', 
                       'mt_code_converter.cpp', 
                       'mt_string_operation.cpp',
                       'mt_special_charset.cpp', 
                       'mt_dictionary.cpp', 
                       'mt_similarity_model.cpp',
                       'mt_log_file.cpp', 
                       "mt_dynamic_programming.cpp"],
             LIBS = ['z', 
                     'mt_htmlparser', 
                     'codedetector' , 
                     'utility_common' , 
                     'wordbreaker' ,
                     'WordSegmentor' , 
                     'dl', 
                     'maxent']);

