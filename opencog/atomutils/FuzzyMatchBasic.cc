/*
 * FuzzyMatchBasic.cc
 *
 * Copyright (C) 2015 OpenCog Foundation
 *
 * Author: Leung Man Hin <https://github.com/leungmanhin>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <opencog/atomspace/Node.h>
#include <opencog/atomutils/AtomUtils.h>
#include <opencog/atomutils/FindUtils.h>

#include "FuzzyMatchBasic.h"

using namespace opencog;

/** Set up the target. */
void FuzzyMatchBasic::start_search(const Handle& trg)
{
	target = trg;
	target_nodes = get_all_nodes(target);
	std::sort(target_nodes.begin(), target_nodes.end());
}

/**
 * hp is a subtree of the target tree. Should we start a search
 * at that location?  Answer: yes if its a node, no, if its a link.
 */
bool FuzzyMatchBasic::accept_starter(const Handle& hp)
{
	NodePtr np(NodeCast(hp));
	if (nullptr == np) return false;

	// Ignore variables. (Why?)
	return (np->getType() != VARIABLE_NODE);
}

/**
 * Estimate how similar the proposed matching tree is to the target.
 *
 * @param soln  The proposed match.
 * @param depth The difference in depth between the target and the
 *              proposal.
 */
bool FuzzyMatchBasic::try_match(const Handle& soln, int depth)
{
	if (soln == target) return false;

	// For some reason, this algo only wnts to compare proposed
	// solutions that are exactly the same size as the target.
	// Why? I dunno. Might not a similar tree be slightly bigger or
	// smaller?  XXX Maybe FIXME ?
	if (0 < depth) return true;
	if (0 > depth) return false;

	// Find out how many nodes it has in common with the pattern
	HandleSeq common_nodes;
	HandleSeq soln_nodes = get_all_nodes(soln);

	std::sort(soln_nodes.begin(), soln_nodes.end());

	std::set_intersection(target_nodes.begin(), target_nodes.end(),
	                      soln_nodes.begin(), soln_nodes.end(),
	                      std::back_inserter(common_nodes));

	// The size different between the pattern and the potential solution
	size_t diff = std::abs((int)target_nodes.size() - (int)soln_nodes.size());

	double similarity = 0.0;

	// Roughly estimate how "rare" each node is by using 1 / incoming set size
	// TODO: May use Truth Value instead
	for (const Handle& common_node : common_nodes)
		similarity += 1.0 / common_node->getIncomingSetSize();

	LAZY_LOG_FINE << "\n========================================\n"
	              << "Comparing:\n" << target->toShortString()
	              << "----- and:\n" << soln->toShortString() << "\n"
	              << "Common nodes = " << common_nodes.size() << "\n"
	              << "Size diff = " << diff << "\n"
	              << "Similarity = " << similarity << "\n"
	              << "Most similar = " << max_similarity << "\n"
	              << "========================================\n";

	// Decide if we should accept the potential solutions or not
	if ((similarity > max_similarity) or
		(similarity == max_similarity and diff < min_size_diff))
	{
		max_similarity = similarity;
		min_size_diff = diff;
		solns.clear();
		solns.push_back({soln, similarity});
	}

	else if (similarity == max_similarity and diff == min_size_diff) {
		solns.push_back({soln, similarity});
	}

	return false;
}

/* No-op; we already build "solns", just return it. */
RankedHandleSeq FuzzyMatchBasic::finished_search(void)
{
	return solns;
}