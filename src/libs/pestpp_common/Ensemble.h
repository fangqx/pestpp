#ifndef ENSEMBLE_H_
#define ENSEMBLE_H_

#include <map>
#include <random>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "FileManager.h"
#include "ObjectiveFunc.h"
#include "OutputFileWriter.h"
#include "PerformanceLog.h"
#include "RunStorage.h"
#include "covariance.h"
#include "RunManagerAbstract.h"
#include "PerformanceLog.h"



class Ensemble
{
public:
	static mt19937_64 rand_engine;
	//Ensemble(Pest &_pest_scenario, FileManager &_file_manager,
	//	OutputFileWriter &_output_file_writer, PerformanceLog *_performance_log, unsigned int seed = 1);
	Ensemble(Pest* _pest_scenario);
	Ensemble() { ; }

	//Ensemble get(vector<string> &_real_names, vector<string> &_var_names);

	void to_csv(string file_name);
	void to_binary_old(string file_name, bool transposed=false);
	void to_binary(string file_name, bool transposed=false);
	void from_eigen_mat(Eigen::MatrixXd mat, const vector<string> &_real_names, const vector<string> &_var_names);
	pair<int, int> shape() { return pair<int, int>(reals.rows(), reals.cols()); }
	void throw_ensemble_error(string message);
	void throw_ensemble_error(string message,vector<string> vec);
	const vector<string> get_var_names() const { return var_names; }
	const vector<string> get_real_names() const { return real_names; }

	const vector<string> get_real_names(vector<int> &indices);

	void extend_cols(Eigen::MatrixXd &_reals, const vector<string> &_var_names);
	void add_2_cols_ip(Ensemble &other);
	void add_2_cols_ip(const vector<string> &_var_names, const Eigen::MatrixXd &mat);
	Ensemble zero_like();

	void reserve(vector<string> _real_names, vector<string> _var_names);

	Eigen::VectorXd get_real_vector(int ireal);
	Eigen::VectorXd get_real_vector(const string &real_name);

	Eigen::MatrixXd get_eigen(vector<string> row_names, vector<string> col_names, bool update_vmap=true);
	const Eigen::MatrixXd get_eigen() const { return reals; }
	const Eigen::MatrixXd* get_eigen_ptr() const { return &reals; }
	void set_eigen(Eigen::MatrixXd _reals);

	Eigen::MatrixXd get_eigen_mean_diff();
	Eigen::MatrixXd get_eigen_mean_diff(const vector<string> &_real_names, const vector<string> &_var_names);

	vector<double> get_mean_stl_vector();
	pair<map<string, double>, map<string, double>>  get_moment_maps(const vector<string> &_real_names=vector<string>());

	void append_other_rows(Ensemble &other);
	void append(string real_name, const Transformable &trans);

	Covariance get_diagonal_cov_matrix();

	void reorder(const vector<string> &_real_names, const vector<string> &_var_names);
	void drop_rows(const vector<int> &row_idxs);
	void drop_rows(const vector<string> &drop_names);
	void keep_rows(const vector<int> &row_idxs);
	void keep_rows(const vector<string> &keep_names);

	Pest* get_pest_scenario_ptr() { return pest_scenario_ptr; }
	Pest get_pest_scenario() { return *pest_scenario_ptr; }
	void set_pest_scenario(Pest *_pest_scenario) { pest_scenario_ptr = _pest_scenario; }
	void set_real_names(vector<string> &_real_names);
	void check_for_dups();

	void draw(int num_reals, Covariance cov, Transformable &tran, const vector<string> &draw_names, const map<string,vector<string>> &grouper, PerformanceLog *plog, int level);
	void update_var_map();
	~Ensemble();
protected:
	Pest* pest_scenario_ptr;
	//FileManager &file_manager;
	//ObjectiveFunc *obj_func_ptr;
	//OutputFileWriter &output_file_writer;
	//PerformanceLog *performance_log;
	string base_name = "BASE";
	Eigen::MatrixXd reals;
	vector<string> var_names;
	vector<string> real_names;	
	map<string, int> var_map;
	void read_csv(int num_reals,ifstream &csv, map<string,int> header_info);
	map<string,int> from_binary_old(string file_name, vector<string> &names,  bool transposed);
	map<string, int> from_binary(string file_name, vector<string> &names, bool transposed);
	map<string,int> prepare_csv(const vector<string> &names, ifstream &csv, bool forgive);
};

class ParameterEnsemble : public Ensemble
{

public:
	enum transStatus { CTL, NUM, MODEL };
	/*ParameterEnsemble(const ParamTransformSeq &_par_transform, Pest &_pest_scenario,
		FileManager &_file_manager,OutputFileWriter &_output_file_writer,
		PerformanceLog *_performance_log, unsigned int seed = 1);
	*/
	ParameterEnsemble(Pest *_pest_scenario_ptr);
	ParameterEnsemble(Pest *_pest_scenario_ptr, Eigen::MatrixXd _reals, vector<string> _real_names, vector<string> _var_names);

	ParameterEnsemble() { ; }
	ParameterEnsemble zeros_like();
	void set_zeros();
	//ParameterEnsemble get_new(const vector<string> &_real_names, const vector<string> &_var_names);

	//void from_csv(string file_name,const vector<string> &ordered_names);
	void from_csv(string file_name);
	void from_binary(string file_name);

	void from_eigen_mat(Eigen::MatrixXd mat, const vector<string> &_real_names, const vector<string> &_var_names,
		transStatus _tstat = transStatus::NUM);
	void enforce_bounds();
	void to_csv(string file_name);
	//Pest* get_pest_scenario_ptr() { return &pest_scenario; }
	transStatus get_trans_status() const { return tstat; }
	void set_trans_status(transStatus _tstat) { tstat = _tstat; }
	ParamTransformSeq get_par_transform() const { return par_transform; }
	void transform_ip(transStatus to_tstat);
	void set_pest_scenario(Pest *_pest_scenario);
	map<int,int> add_runs(RunManagerAbstract *run_mgr_ptr,const vector<int> &real_idxs=vector<int>());

	void draw(int num_reals, Parameters par, Covariance &cov, PerformanceLog *plog, int level);
	Covariance get_diagonal_cov_matrix();
	void to_binary(string filename);

private:
	ParamTransformSeq par_transform;
	transStatus tstat;
	void save_fixed();
	void fill_fixed(const map<string, int> &header_info);
	vector<string> fixed_names;
	map<pair<string, string>, double> fixed_map;
	void replace_fixed(string real_name,Parameters &pars);
};

class ObservationEnsemble : public Ensemble
{
public:
	/*ObservationEnsemble(ObjectiveFunc *_obj_func, Pest &_pest_scenario, FileManager &_file_manager,
    OutputFileWriter &_output_file_writer, PerformanceLog *_performance_log, unsigned int seed = 1);
	*/
	ObservationEnsemble(Pest *_pest_scenario_ptr);
	ObservationEnsemble(Pest *_pest_scenario_ptr, Eigen::MatrixXd _reals, vector<string> _real_names, vector<string> _var_names);

	ObservationEnsemble() { ; }
	void to_binary(string filename) { Ensemble::to_binary(filename, true); }
	void update_from_obs(int row_idx, Observations &obs);
	void update_from_obs(string real_name, Observations &obs);
	//void from_csv(string &file_name, const vector<string> &ordered_names);
	void from_csv(string file_name);
	void from_eigen_mat(Eigen::MatrixXd mat, const vector<string> &_real_names, const vector<string> &_var_names);
	void from_binary(string file_name);// { Ensemble::from_binary(file_name, true); }
	vector<int> update_from_runs(map<int,int> &real_run_ids, RunManagerAbstract *run_mgr_ptr);
	void draw(int num_reals, Covariance &cov, PerformanceLog *plog, int level);

	//ObservationEnsemble get_mean_diff();
};


#endif
