
## return the names of GOs, toghether with root node in a dataframe

get_names=function(go_ids, term_df=NULL, godir=NULL){
    
    # check if term is user-defined or get the integrated version
    term = eval_term(term_df, godir)
    
    # remove obsolete terms
    term = term[term[,5]==0,]
    # find names for GOs
    out = data.frame(go_ids, term[match(go_ids, term[,4]) ,2:3])
    colnames(out) = c("go_id", "go_name", "root_node")
    rownames(out) = 1:nrow(out)
    out[,1] = as.character(out[,1]) 
    return(out)
}
