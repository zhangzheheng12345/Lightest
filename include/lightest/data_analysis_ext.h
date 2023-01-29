/*
This is a Lightest extension, which provides convenient ways to analyze test
data.
*/

#ifndef _DATA_ANALYSIS_H_
#define _DATA_ANALYSIS_H_

#include "lightest.h"

namespace lightest {
void IterAll(const DataSet* data, function<void(const Data*)> func) {
  std::function<void(const lightest::Data*)> iterFunc =                    
        [&iterFunc, &func](const lightest::Data* item) {                             
          if (item->Type() == lightest::DATA_SET)                            
            static_cast<const lightest::DataSet*>(item)->IterSons(iterFunc); 
          else                                                               
            func(item);                                                      
        };                                                                   
    data->IterSons(iterFunc);     
}
void IterFailed(const DataSet* data, function<void(const Data*)> func) {
  std::function<void(const lightest::Data*)> iterFunc =                    
        [&iterFunc, &func](const lightest::Data* item) {
          if(item->GetFailed()) {
            if (item->Type() == lightest::DATA_SET)                            
            static_cast<const lightest::DataSet*>(item)->IterSons(iterFunc); 
          else                                                             
            func(item);   
          }
                                                             
        };                                                                   
    data->IterSons(iterFunc);     
}
};
                                                               

#endif