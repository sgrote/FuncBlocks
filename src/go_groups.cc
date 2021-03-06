
#include "go_groups.h"
#include <cmath>
#include <map> 
#include <set> 
#include <fstream>
#include <iomanip>      // std::setprecision

#include <Rcpp.h>

using namespace std ;

go_groups::go_groups( string &groups, string &sgenes, int co, string root_go ) 
{
	cutoff = co ;

	istringstream is( groups.c_str() ) ;
	string name ;
	while ( is >> name ) {
		names.push_back( name ) ;
		if ( name == root_go ) root_idx = names.size() - 1 ; 
	}
	istringstream is2( sgenes.c_str() ) ;
	int nrgenes_per_group;
	while ( is2 >> nrgenes_per_group ) nr_of_genes.push_back( nrgenes_per_group ) ;

}


int* go_groups::calculate_data( string &data, double sum_nties, ostream *os ) 
{
	istringstream is( data.c_str() ) ;
	int *ret = new int[10] ;
	for ( int i=0 ; i<10 ; ++i ) {
		ret[i] = 0 ;
	}

	while ( is ) {
		double ranksum ;
		is >> ranksum ;
		ranksums.push_back( ranksum ) ;
	}


	data_pvals_l.resize( names.size() ) ;
	data_pvals_g.resize( names.size() ) ;
	ranksums_expected.resize( names.size() ) ;
	

	multiset<double> pvals_less, pvals_greater ;

	for( unsigned int i = 0 ; i < names.size() ; ++i ) {
		
		data_pvals_l[i] = -1. ;
		data_pvals_g[i] = -1. ;

		double N = nr_of_genes[root_idx] ;
		double n = nr_of_genes[i] ;
		// NEW expected sum of ranks ( (root-genes+1)/2 * go-genes )
		ranksums_expected[i] = (N+1.)/2 * n;
		
		N -= n ;
		
		double ranksum = ranksums[i] ;
		
		double prob_greater ;
		double prob_less ;

		if ( n < cutoff ) continue ;

		double C = ranksum - ((n*(n+1.))/2.) ;
		double z = C - (n*N*0.5) ;
		double sigma = sqrt( (n*N/12.)*
				     ( (n+N+1.) - sum_nties / 
					((n+N)*(N+n-1.)) 
				     )
				   ) ; 
		double corr = -0.5 ;
		prob_less = R::pnorm( (z-corr) / sigma, 0., 1., 1, 0 ) ;
		data_pvals_l[i] = prob_less ;
		corr = 0.5 ;
		prob_greater = 1. - R::pnorm( (z-corr) / sigma, 0., 1., 1, 0 ) ;
		data_pvals_g[i] = prob_greater ;

		pvals_less.insert( prob_less ) ;
		pvals_greater.insert( prob_greater ) ;

		if ( os ) {
			*os << names[i] << "\t" 
			   << N << "\t"
			   << n << "\t"
		           << ranksum << endl ;
		}

		if ( prob_less < 0.1 ) {
			ret[0]++ ;
			if ( prob_less < 0.05 ) {
				ret[1]++ ;
				if ( prob_less < 0.01 ) {
					ret[2]++ ;
					if ( prob_less < 0.001 ) {
						ret[3]++ ;
						if ( prob_less < 0.0001 ) {
							ret[4]++ ;
						}
					}
				}
			}
		}
		if ( prob_greater < 0.1 ) {
			ret[5]++ ;
			if ( prob_greater < 0.05 ) {
				ret[6]++ ;
				if ( prob_greater < 0.01 ) {
					ret[7]++ ;
					if ( prob_greater < 0.001 ) {
						ret[8]++ ;
						if ( prob_greater < 0.0001 ) {
							ret[9]++ ;
						}
					}
				}
			}
		}
	}
	return ret ;
}
int* go_groups::calculate_rand( string &data, double sum_nties, ostream *os ) 
{
	istringstream is( data.c_str() ) ;
	int *ret = new int[10] ;
	for ( int i=0 ; i<10 ; ++i ) {
		ret[i] = 0 ;
	}

	vector<double> ranksums_ran;

	while ( is ) {
		double ranksum ;
		is >> ranksum ;
		ranksums_ran.push_back( ranksum ) ;
	}


	multiset<double> pvals_less, pvals_greater ;

	for( unsigned int i = 0 ; i < names.size() ; ++i ) {
		

		double N = nr_of_genes[root_idx] ;
		double n = nr_of_genes[i] ;
		N -= n ;

		double ranksum = ranksums_ran[i] ;		
		double prob_greater ;
		double prob_less ;

		if ( n < cutoff ) continue ;

		double C = ranksum - ((n*(n+1.))/2.) ;
		double z = C - (n*N*0.5) ;
		double sigma = sqrt( (n*N/12.)*
				     ( (n+N+1.) - sum_nties / 
					((n+N)*(N+n-1.)) 
				     )
				   ) ; 
		double corr = -0.5 ;
		prob_less = R::pnorm( (z-corr) / sigma, 0., 1., 1, 0 ) ;
		corr = 0.5 ;
		prob_greater = 1. - R::pnorm( (z-corr) / sigma, 0., 1., 1, 0 ) ;

		pvals_less.insert( prob_less ) ;
		pvals_greater.insert( prob_greater ) ;

		if ( os ) {
			*os << names[i] << "\t" 
			   << prob_less << "\t"
			   << prob_greater << endl ;
		}

		if ( prob_less < 0.1 ) {
			ret[0]++ ;
			if ( prob_less < 0.05 ) {
				ret[1]++ ;
				if ( prob_less < 0.01 ) {
					ret[2]++ ;
					if ( prob_less < 0.001 ) {
						ret[3]++ ;
						if ( prob_less < 0.0001 ) {
							ret[4]++ ;
						}
					}
				}
			}
		}
		if ( prob_greater < 0.1 ) {
			ret[5]++ ;
			if ( prob_greater < 0.05 ) {
				ret[6]++ ;
				if ( prob_greater < 0.01 ) {
					ret[7]++ ;
					if ( prob_greater < 0.001 ) {
						ret[8]++ ;
						if ( prob_greater < 0.0001 ) {
							ret[9]++ ;
						}
					}
				}
			}
		}
	}
	smallest_rand_p_l.insert( *(pvals_less.begin()) ) ;
	smallest_rand_p_g.insert( *(pvals_greater.begin()) ) ;
	return ret ;
}


void go_groups::print_pvals( int nr_randsets, ostream &os ) {

	for( unsigned int i = 0 ; i < names.size() ; ++i ) {
		if ( nr_of_genes[i] >= cutoff ) { 
			// FWER: number of randsets with smallest p <= p-val 
			// for each group in dataset
			int n_l = 0 ; 
			multiset<double>::const_iterator it = smallest_rand_p_l.begin() ;
			while ( it != smallest_rand_p_l.end() && 
				*it <= data_pvals_l[i] + 1.0e-10 * data_pvals_l[i]) // add tolerance to account for float inaccuracy
					n_l++, it++ ;
			int n_g = 0 ;
			it = smallest_rand_p_g.begin() ;
			while ( it != smallest_rand_p_g.end() && 
				*it <= data_pvals_g[i] + 1.0e-10 * data_pvals_g[i]) // add tolerance to account for float inaccuracy 
					n_g++, it++ ;
			// output higher precision for p-vals to check if FWER-order follows p-value-order
			os << std::setprecision(17) << names[i] << "\t" 
				<< data_pvals_l[i] << "\t"
				<< data_pvals_g[i] << "\t"
				<< static_cast<double>(n_l)/static_cast<double>(nr_randsets) << "\t" //FWER low ranks
				<< static_cast<double>(n_g)/static_cast<double>(nr_randsets) << "\t" //FWER high ranks
				<< ranksums_expected[i] << "\t" << ranksums[i]  //expected and real sum of ranks
				<< endl;
		}
	} 
}

void go_groups::print_min_p( ostream &os ) {
	
	multiset<double>::const_iterator it_l = smallest_rand_p_l.begin() ;
	multiset<double>::const_iterator it_r = smallest_rand_p_g.begin() ;
	while ( it_l != smallest_rand_p_l.end() ) { 
		os << std::setprecision(17) << *it_l << "\t" << *it_r << endl;
		it_l++, it_r++ ;
	}
}
