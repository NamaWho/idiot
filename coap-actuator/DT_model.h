


    // !!! This file is generated using emlearn !!!

    #include <eml_trees.h>
    

static const EmlTreesNode model_nodes[69] = {
  { 7, 0.500000f, 1, 36 },
  { 8, 0.500000f, 1, 29 },
  { 6, 0.500000f, 1, 23 },
  { 4, 0.500000f, -1, 1 },
  { 0, 184.237152f, -1, 1 },
  { 2, 44.166655f, 1, -1 },
  { 3, 101.591053f, 1, 14 },
  { 2, 40.042463f, 1, 11 },
  { 2, 39.908703f, 1, 8 },
  { 0, 190.179832f, 1, -2 },
  { 0, 189.557457f, 1, 5 },
  { 0, 189.121628f, 1, 3 },
  { 3, 97.057320f, 1, -2 },
  { 3, 96.800453f, -2, -1 },
  { 2, 38.841761f, -2, -1 },
  { 3, 100.290524f, -1, -2 },
  { 1, 447.544556f, -2, 1 },
  { 3, 99.401348f, -2, -1 },
  { 3, 97.695076f, 1, -2 },
  { 3, 97.585888f, -2, -1 },
  { 1, 442.263031f, 1, 2 },
  { 0, 193.112663f, -1, -2 },
  { 0, 185.710182f, -1, 1 },
  { 2, 40.423315f, -2, 1 },
  { 2, 40.486790f, -1, -2 },
  { 1, 393.231583f, 1, -1 },
  { 5, 0.500000f, -1, 1 },
  { 0, 189.563286f, -3, 1 },
  { 1, 371.890503f, -2, 1 },
  { 1, 376.784714f, -2, -3 },
  { 2, 45.380051f, -1, 1 },
  { 5, 0.500000f, 1, 4 },
  { 0, 186.073769f, -4, 1 },
  { 4, 0.500000f, -4, 1 },
  { 0, 191.922569f, -2, -2 },
  { 3, 97.337013f, -3, -3 },
  { 3, 118.776756f, 1, 12 },
  { 8, 0.500000f, 1, 9 },
  { 1, 374.769669f, 1, 2 },
  { 3, 100.056545f, -1, -3 },
  { 0, 185.759331f, -1, 1 },
  { 4, 0.500000f, -1, 1 },
  { 2, 39.051371f, -2, 1 },
  { 0, 186.735840f, -2, 1 },
  { 3, 99.769753f, -1, 1 },
  { 1, 452.255768f, -1, -2 },
  { 3, 100.543861f, 1, -4 },
  { 3, 99.962639f, -3, -3 },
  { 5, 0.500000f, 1, 16 },
  { 4, 0.500000f, 1, 11 },
  { 3, 122.706161f, 1, 6 },
  { 3, 122.681889f, 1, -1 },
  { 1, 432.511063f, 1, 2 },
  { 3, 121.515694f, -1, -5 },
  { 3, 122.415707f, -5, 1 },
  { 0, 170.175934f, -1, -5 },
  { 2, 48.953701f, 1, 3 },
  { 1, 421.090134f, 1, -5 },
  { 1, 420.126511f, -5, -1 },
  { 2, 49.146833f, -5, -5 },
  { 0, 185.299141f, 1, 2 },
  { 0, 177.259605f, -5, -2 },
  { 2, 38.439026f, 1, -2 },
  { 0, 189.392799f, -2, -2 },
  { 6, 0.500000f, -5, 1 },
  { 0, 168.694221f, 1, 3 },
  { 0, 168.409035f, -3, 1 },
  { 2, 39.302877f, -3, -3 },
  { 0, 173.046577f, -3, -3 } 
};

static const int32_t model_tree_roots[1] = { 0 };

static const uint8_t model_leaves[5] = { 4, 0, 1, 3, 2 };

EmlTrees model = {
        69,
        (EmlTreesNode *)(model_nodes),	  
        1,
        (int32_t *)(model_tree_roots),
        5,
        (uint8_t *)(model_leaves),
        0,
        9,
        5,
    };

static inline int32_t model_tree_0(const float *features, int32_t features_length) {
          if (features[7] < 0.500000f) {
              if (features[8] < 0.500000f) {
                  if (features[6] < 0.500000f) {
                      if (features[4] < 0.500000f) {
                          return 4;
                      } else {
                          if (features[0] < 184.237152f) {
                              return 4;
                          } else {
                              if (features[2] < 44.166655f) {
                                  if (features[3] < 101.591053f) {
                                      if (features[2] < 40.042463f) {
                                          if (features[2] < 39.908703f) {
                                              if (features[0] < 190.179832f) {
                                                  if (features[0] < 189.557457f) {
                                                      if (features[0] < 189.121628f) {
                                                          if (features[3] < 97.057320f) {
                                                              if (features[3] < 96.800453f) {
                                                                  return 0;
                                                              } else {
                                                                  return 4;
                                                              }
                                                          } else {
                                                              return 0;
                                                          }
                                                      } else {
                                                          if (features[2] < 38.841761f) {
                                                              return 0;
                                                          } else {
                                                              return 4;
                                                          }
                                                      }
                                                  } else {
                                                      if (features[3] < 100.290524f) {
                                                          return 4;
                                                      } else {
                                                          return 0;
                                                      }
                                                  }
                                              } else {
                                                  return 0;
                                              }
                                          } else {
                                              if (features[1] < 447.544556f) {
                                                  return 0;
                                              } else {
                                                  if (features[3] < 99.401348f) {
                                                      return 0;
                                                  } else {
                                                      return 4;
                                                  }
                                              }
                                          }
                                      } else {
                                          if (features[3] < 97.695076f) {
                                              if (features[3] < 97.585888f) {
                                                  return 0;
                                              } else {
                                                  return 4;
                                              }
                                          } else {
                                              return 0;
                                          }
                                      }
                                  } else {
                                      if (features[1] < 442.263031f) {
                                          if (features[0] < 193.112663f) {
                                              return 4;
                                          } else {
                                              return 0;
                                          }
                                      } else {
                                          if (features[0] < 185.710182f) {
                                              return 4;
                                          } else {
                                              if (features[2] < 40.423315f) {
                                                  return 0;
                                              } else {
                                                  if (features[2] < 40.486790f) {
                                                      return 4;
                                                  } else {
                                                      return 0;
                                                  }
                                              }
                                          }
                                      }
                                  }
                              } else {
                                  return 4;
                              }
                          }
                      }
                  } else {
                      if (features[1] < 393.231583f) {
                          if (features[5] < 0.500000f) {
                              return 4;
                          } else {
                              if (features[0] < 189.563286f) {
                                  return 1;
                              } else {
                                  if (features[1] < 371.890503f) {
                                      return 0;
                                  } else {
                                      if (features[1] < 376.784714f) {
                                          return 0;
                                      } else {
                                          return 1;
                                      }
                                  }
                              }
                          }
                      } else {
                          return 4;
                      }
                  }
              } else {
                  if (features[2] < 45.380051f) {
                      return 4;
                  } else {
                      if (features[5] < 0.500000f) {
                          if (features[0] < 186.073769f) {
                              return 3;
                          } else {
                              if (features[4] < 0.500000f) {
                                  return 3;
                              } else {
                                  if (features[0] < 191.922569f) {
                                      return 0;
                                  } else {
                                      return 0;
                                  }
                              }
                          }
                      } else {
                          if (features[3] < 97.337013f) {
                              return 1;
                          } else {
                              return 1;
                          }
                      }
                  }
              }
          } else {
              if (features[3] < 118.776756f) {
                  if (features[8] < 0.500000f) {
                      if (features[1] < 374.769669f) {
                          if (features[3] < 100.056545f) {
                              return 4;
                          } else {
                              return 1;
                          }
                      } else {
                          if (features[0] < 185.759331f) {
                              return 4;
                          } else {
                              if (features[4] < 0.500000f) {
                                  return 4;
                              } else {
                                  if (features[2] < 39.051371f) {
                                      return 0;
                                  } else {
                                      if (features[0] < 186.735840f) {
                                          return 0;
                                      } else {
                                          if (features[3] < 99.769753f) {
                                              return 4;
                                          } else {
                                              if (features[1] < 452.255768f) {
                                                  return 4;
                                              } else {
                                                  return 0;
                                              }
                                          }
                                      }
                                  }
                              }
                          }
                      }
                  } else {
                      if (features[3] < 100.543861f) {
                          if (features[3] < 99.962639f) {
                              return 1;
                          } else {
                              return 1;
                          }
                      } else {
                          return 3;
                      }
                  }
              } else {
                  if (features[5] < 0.500000f) {
                      if (features[4] < 0.500000f) {
                          if (features[3] < 122.706161f) {
                              if (features[3] < 122.681889f) {
                                  if (features[1] < 432.511063f) {
                                      if (features[3] < 121.515694f) {
                                          return 4;
                                      } else {
                                          return 2;
                                      }
                                  } else {
                                      if (features[3] < 122.415707f) {
                                          return 2;
                                      } else {
                                          if (features[0] < 170.175934f) {
                                              return 4;
                                          } else {
                                              return 2;
                                          }
                                      }
                                  }
                              } else {
                                  return 4;
                              }
                          } else {
                              if (features[2] < 48.953701f) {
                                  if (features[1] < 421.090134f) {
                                      if (features[1] < 420.126511f) {
                                          return 2;
                                      } else {
                                          return 4;
                                      }
                                  } else {
                                      return 2;
                                  }
                              } else {
                                  if (features[2] < 49.146833f) {
                                      return 2;
                                  } else {
                                      return 2;
                                  }
                              }
                          }
                      } else {
                          if (features[0] < 185.299141f) {
                              if (features[0] < 177.259605f) {
                                  return 2;
                              } else {
                                  return 0;
                              }
                          } else {
                              if (features[2] < 38.439026f) {
                                  if (features[0] < 189.392799f) {
                                      return 0;
                                  } else {
                                      return 0;
                                  }
                              } else {
                                  return 0;
                              }
                          }
                      }
                  } else {
                      if (features[6] < 0.500000f) {
                          return 2;
                      } else {
                          if (features[0] < 168.694221f) {
                              if (features[0] < 168.409035f) {
                                  return 1;
                              } else {
                                  if (features[2] < 39.302877f) {
                                      return 1;
                                  } else {
                                      return 1;
                                  }
                              }
                          } else {
                              if (features[0] < 173.046577f) {
                                  return 1;
                              } else {
                                  return 1;
                              }
                          }
                      }
                  }
              }
          }
        }
        

int32_t model_predict(const float *features, int32_t features_length) {

        int32_t votes[5] = {0,};
        int32_t _class = -1;

        _class = model_tree_0(features, features_length); votes[_class] += 1;
    
        int32_t most_voted_class = -1;
        int32_t most_voted_votes = 0;
        for (int32_t i=0; i<5; i++) {

            if (votes[i] > most_voted_votes) {
                most_voted_class = i;
                most_voted_votes = votes[i];
            }
        }
        return most_voted_class;
    }
    